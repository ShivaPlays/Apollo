#include "audio/music.h"

#include <stdexcept>
#include <chrono>
#include <iostream>

#include "audio/device.h"
#include "audio/stream_factory.h"
#include "audio/reserved_channel.h"
#include "system/asset_istream.h"
#include "system/mem_istream.h"

namespace age::audio
{
	music::music()
		: m_requested_stream{ nullptr }
		, m_internal_state{ state::stopped }
		, m_alive{ true }
	{
		m_samples_buffer.resize(BUFFER_SAMPLES);
		set_relative_to_listener(true);
		set_direct_channels(true);

		m_background_worker.add_job([this](){ worker_loop(); });
	}

	music::~music()
	{
		m_alive = false;
		music::stop();
	}

	void music::play()
	{
		m_internal_state.store(state::playing, std::memory_order_relaxed);
		add_command(command{command::type::play, get_properties().looping});
	}

	void music::stop()
	{
		m_internal_state.store(state::stopped, std::memory_order_relaxed);
		get_channel_link().stop();

		add_command(command{command::type::stop});
	}

	void music::pause()
	{
		m_internal_state.store(state::paused, std::memory_order_relaxed);
		get_channel_link().pause();
	}

	void music::on_channel_lost(channel& channel)
	{
		channel.stop();
		channel.clear_buffers();

		m_internal_state.store(state::stopped, std::memory_order_relaxed);

		//Also notify the worker thread that the music has stopped
		add_command(command{command::type::stop});
	}

	void music::on_queued_buffers_processed(size_t num_buffers)
	{
		//We need to generate a command for the worker thread that there is more data to buffer
		add_command(command{command::type::buffer, num_buffers});
	}

	void music::update_looping(bool value)
	{
		set_looping(value);
	}

	void music::open(std::string_view fn)
	{
		stop();

		command cmd;
		cmd.cmd_type = command::type::open;
		auto& path_arr = cmd.data.emplace<command::path_array>();
		path_arr.fill(0);
		fn.copy(path_arr.data(), path_arr.size() - 1);

		add_command(std::move(cmd));
	}

	void music::open(std::istream& is)
	{
		stop();

		add_command(command{command::type::open, &is});
	}

	void music::open(std::shared_ptr<std::istream> is)
	{
		stop();

		add_command(command{command::type::open, std::move(is)});
	}

	void music::open(std::byte data[], size_t size)
	{
		stop();

		add_command(command{command::type::open, std::pair{data, size}});
	}

	state music::get_state() const
	{
		return m_internal_state.load(std::memory_order_relaxed);
	}

	void music::open_from_stream(std::istream& is)
	{
		stop();

		m_sound_stream_info = stream::info{};
		m_sound_stream = stream_factory::create_from_stream(is);

		if (!m_sound_stream)
		{
			throw std::runtime_error{ "Unable to determine audio type from stream" };
		}
			
		m_sound_stream_info = m_sound_stream->open(is);
	}

	void music::worker_loop()
	{
		bool play_looped = false;

		while (m_alive)
		{
			//Lets see if we have a command waiting in our queue
			std::unique_lock lock{ m_command_mutex };
			m_command_cv.wait(lock, [this]()->bool { return !m_command_buffer.empty(); });

			auto cmd = m_command_buffer.pop();

			switch (cmd.cmd_type)
			{
			case command::type::open:
				{
					if (auto* path_arr = std::get_if<command::path_array>(&cmd.data))
					{
						std::string_view path{path_arr->data()};
						m_active_istream.reset();
						m_active_istream = std::make_unique<asset_istream>(path.data(), std::ios::binary);
						m_requested_stream = nullptr;
					}
					else if (auto* stream_ptr = std::get_if<std::istream*>(&cmd.data))
					{
						m_active_istream.reset();
						m_requested_stream = *stream_ptr;
					}
					else if (auto* shared_stream = std::get_if<std::shared_ptr<std::istream>>(&cmd.data))
					{
						m_active_istream = std::move(*shared_stream);
						m_requested_stream = nullptr;
					}
					else if (auto* raw = std::get_if<std::pair<std::byte*, size_t>>(&cmd.data))
					{
						m_active_istream.reset();
						m_active_istream = std::make_unique<mem_istream>(raw->first, raw->second);
						m_requested_stream = nullptr;
					}
					else
					{
						m_active_istream.reset();
						m_requested_stream = nullptr;
					}

					if (std::istream* target = m_active_istream ? m_active_istream.get() : m_requested_stream)
					{
						m_sound_stream_info = stream::info{};
						m_sound_stream.reset();
						m_sound_stream = stream_factory::create_from_stream(*target);

						if (!m_sound_stream) continue;

						m_sound_stream_info = m_sound_stream->open(*target);
					}
				}
				break;

			case command::type::play:
				{
					//Play was called without any data calling open first. So there is no data to stream
					if (!m_active_istream && !m_requested_stream)
					{
						m_internal_state.store(state::stopped, std::memory_order_relaxed);
						continue;
					}

					//There is an easy way for us determine if we are resuming a music or if we need to buffer data
					//When we have an attached source, the buffering has already been done, and we can just play.
					//If there is no attaches source we need to get one and buffer some data first
					if (!has_channel())
					{
						auto guard = device::get().request_channel();
						//If no source is available, this means there are already many sounds playing and we can just stop here
						if (!guard)
						{
							m_internal_state.store(state::stopped, std::memory_order_relaxed);
							continue;
						}

						guard->clear_buffers();
						attach_channel(reserved_channel{ guard });

						//Okay we got a sound_source, lets buffer some data
						m_sound_stream->reset();
						for (auto& buffer : m_buffers)
						{
							size_t read = m_sound_stream->read(&m_samples_buffer[0], m_samples_buffer.size());
							if (read == 0) { break; }

							buffer::format format = (m_sound_stream_info.channel_count == 1) ?
								buffer::format::mono_16 : buffer::format::stereo_16;
							buffer.buffer_data(format, &m_samples_buffer[0], read, m_sound_stream_info.sample_rate);
							guard->enqueue_buffer(buffer);
						}
					}

					get_channel_link().execute_on_channel([this](channel& chan) {
						if (chan.get_state() != state::playing)
						{
							auto props = get_properties();
							props.looping = false;

							chan.set_auxiliary_bus(get_auxiliary_bus());
							chan.play(props);
							m_internal_state = chan.get_state();
						}
					});

					if (auto looped = std::get_if<bool>(&cmd.data)) play_looped = *looped;
				}
				break;

			case command::type::stop:
				{
					get_channel_link().clear_buffers();
					detach_channel();
				}
				break;

			case command::type::buffer:
				{
					auto num_processed_buffers = std::get<std::size_t>(cmd.data);

					get_channel_link().execute_on_channel([this, play_looped, &num_processed_buffers](channel& chan)
					{
						bool no_more_data = false;
						while (num_processed_buffers--)
						{
							size_t bytes_read = m_sound_stream->read(&m_samples_buffer[0], m_samples_buffer.size());

							//When there are fewer bytes read than requested, the stream has finished.
							//When looped the file actually needs to be read again from the beginning and the buffer can be filled a bit more
							if (bytes_read < m_samples_buffer.size())
							{
								//When not looping and no more bytes read, we are finished here. If not lets read a bit more until the stream is finished
								if (!play_looped)
								{
									if (bytes_read == 0)
									{
										no_more_data = true;
										break;
									}
								}
								else
								{
									//Here we are when we want to loop. We just reset the stream and start a new
									m_sound_stream->reset();
									size_t difference = m_samples_buffer.size() - bytes_read;
									bytes_read += m_sound_stream->read(&m_samples_buffer[bytes_read], difference);
								}
							}

							auto processed_buffer = chan.unqueue_buffer();
							buffer::format format = m_sound_stream_info.channel_count == 1
														? buffer::format::mono_16
														: buffer::format::stereo_16;
							processed_buffer.buffer_data(format, &m_samples_buffer[0], bytes_read,
														 m_sound_stream_info.sample_rate);

							chan.enqueue_buffer(processed_buffer);
						}

						if (chan.get_state() == state::stopped && chan.get_num_queued_buffers() > 0)
						{
							//std::cout << "Music: buffer_queue underrun: Recovering!\n";
							chan.play();
						}

						if (!play_looped && no_more_data && chan.get_state() == state::stopped)
						{
							//When we reach here, we actually want to clean up and set the state to stopped.
							stop();
						}
					});

				}
				break;

			default: break;

			}
		}
	}
}
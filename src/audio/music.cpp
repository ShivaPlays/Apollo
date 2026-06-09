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
		m_command_queue.reserve(8);
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
		m_internal_state = state::playing;

		std::lock_guard lock{ m_command_mutex };
		m_command_queue.emplace_back(music_command{music_command::type::play, get_properties().looping});
		m_command_cv.notify_one();
	}

	void music::stop()
	{
		m_internal_state = state::stopped;
		get_channel_link().stop();

		std::lock_guard lock{ m_command_mutex };
		m_command_queue.emplace_back(music_command{music_command::type::stop});
		m_command_cv.notify_one();
	}

	void music::pause()
	{
		m_internal_state = state::paused;
		get_channel_link().pause();

		std::lock_guard lock{ m_command_mutex };
		m_command_queue.emplace_back(music_command{music_command::type::pause});
		m_command_cv.notify_one();
	}

	void music::on_channel_lost()
	{
		stop();
	}

	void music::update_looping(bool value)
	{
		set_looping(value);
	}

	void music::open(std::string_view fn)
	{
		stop();

		music_command cmd;
		cmd.cmd_type = music_command::type::open;
		auto& path_arr = cmd.data.emplace<std::array<char, 256>>();
		path_arr.fill(0);
		fn.copy(path_arr.data(), path_arr.size() - 1);

		std::lock_guard lock{ m_command_mutex };

		m_command_queue.push_back(std::move(cmd));
		m_command_cv.notify_one();
	}

	void music::open(std::istream& is)
	{
		stop();

		std::lock_guard lock{ m_command_mutex };

		m_command_queue.emplace_back(music_command{music_command::type::open, &is});
		m_command_cv.notify_one();
	}

	void music::open(std::unique_ptr<std::istream> is)
	{
		stop();

		std::lock_guard lock{ m_command_mutex };

		m_command_queue.emplace_back(music_command{music_command::type::open, std::move(is)});
		m_command_cv.notify_one();
	}

	void music::open(std::byte data[], size_t size)
	{
		stop();

		std::lock_guard lock{ m_command_mutex };
		m_command_queue.emplace_back(music_command{music_command::type::open, std::pair{data, size}});
		m_command_cv.notify_one();
	}

	state music::get_state() const
	{
		return m_internal_state;
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
		bool play_requested = false;
		bool play_looped = false;

		while (m_alive)
		{
			music_command cmd{};
			bool has_work = false;

			{
				std::unique_lock lock{ m_command_mutex };

				if (play_requested)
				{
					// Wait for a NEW command, but only for 50ms.
					// If 50ms pass without a command, it returns 'false' and we continue to refill.
					m_command_cv.wait_for(lock, std::chrono::milliseconds(50), [this]()->bool { return !m_command_queue.empty(); });
				}
				else
				{
					// If not playing, wait indefinitely for a command.
					m_command_cv.wait(lock, [this]()->bool { return !m_command_queue.empty(); });
				}

				if (!m_command_queue.empty())
				{
					cmd = std::move(m_command_queue.front());
					m_command_queue.erase(m_command_queue.begin());
					has_work = true;
				}
			}

			if (has_work)
			{
				play_requested = false;

				switch (cmd.cmd_type)
				{
					case music_command::type::open:
					{
						if (auto* path_arr = std::get_if<std::array<char, 256>>(&cmd.data))
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
						else if (auto* unique_stream = std::get_if<std::unique_ptr<std::istream>>(&cmd.data))
						{
							m_active_istream = std::move(*unique_stream);
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

					case music_command::type::play:
					{
						//Play was called without any data calling open first. So there is no data to stream
						if (!m_active_istream && !m_requested_stream)
						{
							m_internal_state = state::stopped;
							continue;
						}

						//There is an easy way for us determine if we are resuming a music or if we need to buffer data
						//When we have an attached source, the buffering has already been done, and we can just play.
						//If there is no attaches source we need to get one and buffer some data first
						if (!has_channel())
						{
							auto guard = device::get().request_channel();
							//If nor source is available, this means there are already many sounds playing and we can just stop here
							if (!guard)
							{
								m_internal_state = state::stopped;
								continue;
							}

							guard->clear_buffers();
							attach_channel(reserved_channel{ guard });

							//Okay we got a sound_source, lets buffer some data
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
							}
						});

						play_requested = true;

						if (auto looped = std::get_if<bool>(&cmd.data)) play_looped = *looped;
					}
					break;

					case music_command::type::stop:
					{
						m_internal_state = state::stopped;

						get_channel_link().clear_buffers();
						detach_channel();
					}
					break;

					case music_command::type::pause:
					{
						m_internal_state = state::paused;
					}
					break;

					default: break;
				}
			}
			else if (play_requested)
			{
				get_channel_link().execute_on_channel([this, play_looped](channel& chan) {
					bool no_more_data = false;

					//If no command has arrived and play_requested is true then lets continue buffering the source
					auto processed_buffers = chan.get_num_processed_buffers();
					//std::cout << "processed buffers: " << processed_buffers << "\n";
					while (processed_buffers--)
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
						//We can do that easily by just adding a command
						std::unique_lock lock{m_command_mutex};
						m_command_queue.emplace_back(music_command{music_command::type::stop, false});
						m_command_cv.notify_one();
					}
				});
			}
		}
	}
}
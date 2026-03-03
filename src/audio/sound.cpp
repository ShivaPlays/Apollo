#include "audio/sound.h"

#include "audio/sound_source.h"
#include "audio/audio_device.h"

namespace age
{
	sound::sound()
		: m_buffer{ nullptr }
	{}

	sound::~sound()
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
		{
			current_attached_channel->set_owner(nullptr);

			auto source_looping = current_attached_channel->get_source().get_looping();

			//Our source is in the unavailable container. Make it available again
			if (source_looping || current_attached_channel->get_source().get_state() == sound_state::paused)
			{
				current_attached_channel->get_source().stop();
				current_attached_channel->set_reserved(false);
			}
		}
	}

	void sound::play(bool looped)
	{
		if (!m_buffer) return;

		auto current_attached_channel = get_attached_channel();
		if (current_attached_channel)
		{
			if (current_attached_channel->get_source().get_state() == sound_state::paused)
			{
				current_attached_channel->get_source().play();
				return;
			}

			if (current_attached_channel->get_source().get_looping())
			{
				current_attached_channel->get_source().stop();
				current_attached_channel->get_source().set_buffer(*m_buffer);
				current_attached_channel->get_source().set_looping(looped);
				current_attached_channel->get_source().play();

				return;
			}
		}

		auto properties = get_properties();
		properties.looping = looped;

		auto* new_channel = audio_device::get().play_buffer(*m_buffer, properties);

		if (new_channel)
		{
			attach_channel(new_channel);
		}
	}

	void sound::stop()
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
		{
			if (current_attached_channel->get_source().get_looping())
			{
				current_attached_channel->set_reserved(false);
			}

			current_attached_channel->get_source().stop();
		}
	}

	void sound::pause()
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
			current_attached_channel->get_source().pause();
	}

	sound_state sound::get_state() const
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
			return current_attached_channel->get_source().get_state();

		return sound_state::stopped;
	}

	void sound::set_buffer(const sound_buffer* value)
	{
		m_buffer = value;
	}

	const sound_buffer* sound::get_buffer() const
	{
		return m_buffer;
	}
}
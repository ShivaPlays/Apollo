#include "audio/sound.h"

#include "audio/source.h"
#include "audio/device.h"

namespace age::audio
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

			auto source_looping = current_attached_channel->get_looping();

			//Our source is in the unavailable container. Make it available again
			if (source_looping || current_attached_channel->get_state() == state::paused)
			{
				current_attached_channel->stop();
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
			if (current_attached_channel->get_state() == state::paused)
			{
				current_attached_channel->play();
				return;
			}

			if (current_attached_channel->get_looping())
			{
				current_attached_channel->stop();
				current_attached_channel->attach_buffer(*m_buffer);
				current_attached_channel->set_looping(looped);
				current_attached_channel->play();

				return;
			}
		}

		auto properties = get_properties();
		properties.looping = looped;

		auto guard = device::get().play_buffer(*m_buffer, properties);
		if (guard) attach_channel(guard);
	}

	void sound::stop()
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
		{
			if (current_attached_channel->get_looping())
			{
				current_attached_channel->set_reserved(false);
			}

			current_attached_channel->stop();
		}
	}

	void sound::pause()
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
			current_attached_channel->pause();
	}

	state sound::get_state() const
	{
		auto current_attached_channel = get_attached_channel();

		if (current_attached_channel)
			return current_attached_channel->get_state();

		return state::stopped;
	}

	void sound::set_buffer(const buffer* value)
	{
		m_buffer = value;
	}

	const buffer* sound::get_buffer() const
	{
		return m_buffer;
	}
}
#include "audio/sound_interface.h"

#include "audio/source.h"
#include "audio/device.h"

namespace age::audio
{
	sound_interface::sound_interface(const sound_interface& other)
		: m_properties{ other.m_properties }
	{}

	sound_interface::sound_interface(sound_interface&& other) noexcept
		: m_properties{ other.m_properties }
	{
		// We lock 'other' to ensure it isn't being updated by the audio thread
		// while we are stealing its channel.
		auto* chan = other.m_attached_channel.exchange(nullptr, std::memory_order_acq_rel);
		m_attached_channel.store(chan, std::memory_order_release);

		if (chan) chan->set_owner(this);
	}

	sound_interface& sound_interface::operator = (const sound_interface& other)
	{
		m_properties = other.m_properties;

		return *this;
	}

	sound_interface& sound_interface::operator = (sound_interface&& other) noexcept
	{
		if (this == &other) return *this;

		// 1. Clean up our current state
		detach_channel();

		// 2. Transfer properties
		m_properties = other.m_properties;

		// 3. Transfer channel ownership
		auto* chan = other.m_attached_channel.exchange(nullptr, std::memory_order_acq_rel);
		m_attached_channel.store(chan, std::memory_order_release);

		if (chan) chan->set_owner(this);

		return *this;
	}


	bool sound_interface::get_looping() const
	{
		std::lock_guard lock{ m_channel_mutex };

		if (m_attached_channel)
			return m_attached_channel->get_source().get_looping();

		return false;
	}

	void sound_interface::attach_channel(channel* value)
	{
		detach_channel();

		if (value)
		{
			m_attached_channel.store(value, std::memory_order_release);
			value->set_owner(this);
		}
	}

	channel* sound_interface::get_attached_channel() const
	{
		return m_attached_channel;
	}

	void sound_interface::detach_channel() const
	{
		// exchange(nullptr) is atomic: it grabs the pointer and sets the member to null
		// in one single CPU instruction.
		if (auto* chan = m_attached_channel.exchange(nullptr, std::memory_order_acq_rel))
		{
			// Now only THIS thread is responsible for notifying the channel
			chan->set_owner(nullptr);
		}
	}

	const properties & sound_interface::get_properties() const
	{
		return m_properties;
	}
}

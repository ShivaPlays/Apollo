#include "audio/sound_interface.h"

#include "audio/source.h"
#include "audio/device.h"

namespace age::audio
{
	sound_interface::sound_interface(const sound_interface& other)
		: m_properties{ other.m_properties }
		, m_attached_channel{ nullptr }
	{}

	sound_interface::sound_interface(sound_interface&& other) noexcept
		: m_properties{ other.m_properties }
		, m_attached_channel{ other.m_attached_channel }
	{
		// We lock 'other' to ensure it isn't being updated by the audio thread
		// while we are stealing its channel.
		std::lock_guard lock{ other.m_channel_mutex };

		m_attached_channel = other.m_attached_channel;
		other.m_attached_channel = nullptr;

		if (m_attached_channel) {
			m_attached_channel->set_owner(this);
		}
	}

	sound_interface& sound_interface::operator = (const sound_interface& other)
	{
		m_properties = other.m_properties;

		return *this;
	}

	sound_interface& sound_interface::operator = (sound_interface&& other) noexcept
	{
		if (this == &other) return *this;

		// Use scoped_lock to safely lock both mutexes without risk of deadlock
		std::scoped_lock lock{ m_channel_mutex, other.m_channel_mutex };

		// 1. Clean up our current state
		detach_channel_locked();

		// 2. Transfer properties
		m_properties = other.m_properties;

		// 3. Transfer channel ownership
		m_attached_channel = other.m_attached_channel;
		other.m_attached_channel = nullptr;

		if (m_attached_channel) {
			m_attached_channel->set_owner(this);
		}

		return *this;
	}

	void sound_interface::stop()
	{
		std::lock_guard lock{ m_channel_mutex };

		if (m_attached_channel)
		{
			if (m_attached_channel->get_source().get_looping())
				m_attached_channel->set_reserved(false);

			m_attached_channel->get_source().stop();
		}
	}

	void sound_interface::update_source(source& source, bool looped) const
	{
		source.apply_properties(m_properties);
		source.set_looping(looped);
	}

	void sound_interface::set_position(const glm::vec3& value)
	{
		m_properties.position = value;
	}

	void sound_interface::update_position(const glm::vec3& value)
	{
		set_position(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_position(value);
	}

	const glm::vec3& sound_interface::get_position() const
	{
		return m_properties.position;
	}

	void sound_interface::set_pitch(float value)
	{
		m_properties.pitch = value;
	}

	void sound_interface::update_pitch(float value)
	{
		set_pitch(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_pitch(value);
	}

	float sound_interface::get_pitch() const
	{
		return m_properties.pitch;
	}

	void sound_interface::set_volume(float value)
	{
		m_properties.volume = value;
	}

	void sound_interface::update_volume(float value)
	{
		set_volume(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_volume(value);
	}

	float sound_interface::get_volume() const
	{
		return m_properties.volume;
	}

	void sound_interface::set_reference_distance(float value)
	{
		m_properties.reference_distance = value;
	}

	void sound_interface::update_reference_distance(float value)
	{
		set_reference_distance(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_reference_distance(value);
	}

	float sound_interface::get_reference_distance() const
	{
		return m_properties.reference_distance;
	}

	void sound_interface::set_rolloff_factor(float value)
	{
		m_properties.rolloff_factor = value;
	}

	void sound_interface::update_rolloff_factor(float value)
	{
		set_rolloff_factor(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_rolloff_factor(value);
	}

	float sound_interface::get_rolloff_factor() const
	{
		return m_properties.rolloff_factor;
	}

	void sound_interface::set_relative_to_listener(bool value)
	{
		m_properties.relative_to_listener = value;
	}

	void sound_interface::update_relative_to_listener(bool value)
	{
		set_relative_to_listener(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->get_source().set_relative_to_listener(value);
	}

	bool sound_interface::get_relative_to_listener() const
	{
		return m_properties.relative_to_listener;
	}

	void sound_interface::set_direct_channels(bool value)
	{
		m_properties.direct_channels = value;
	}

	void sound_interface::update_direct_channels(bool value)
	{
		set_direct_channels(value);

		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel && device::get().is_direct_channels_available()) m_attached_channel->get_source().set_direct_channels(value);
	}

	bool sound_interface::get_direct_channels() const
	{
		return m_properties.direct_channels;
	}

	void sound_interface::set_auxiliary_bus(uint8_t value)
	{
		m_auxiliary_bus = value;
	}

	void sound_interface::update_auxiliary_bus(uint8_t value)
	{
		set_auxiliary_bus(value);
		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel) m_attached_channel->set_auxiliary_bus(value);
	}

	uint8_t sound_interface::get_auxiliary_bus() const
	{
		return m_auxiliary_bus;
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
		std::lock_guard lock{ m_channel_mutex };
		if (m_attached_channel == value) return;

		detach_channel_locked();
		m_attached_channel = value;
		if (m_attached_channel)
			m_attached_channel->set_owner(this);
	}

	channel* sound_interface::get_attached_channel() const
	{
		return m_attached_channel;
	}

	void sound_interface::detach_channel() const
	{
		std::lock_guard lock{ m_channel_mutex };

		detach_channel_locked();
	}

	void sound_interface::detach_channel_locked() const
	{
		if (m_attached_channel)
		{
			m_attached_channel->set_owner(nullptr);
			m_attached_channel = nullptr;
		}
	}

	const properties & sound_interface::get_properties() const
	{
		return m_properties;
	}
}

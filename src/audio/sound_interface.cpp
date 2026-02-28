#include "audio/sound_interface.h"

#include "audio/sound_source.h"
#include "audio/audio_device.h"

namespace age
{
	sound_interface::sound_interface(const sound_interface& other)
		: m_properties{ other.m_properties }
		, m_attached_source{ nullptr }
	{}

	sound_interface::sound_interface(sound_interface&& other) noexcept
		: m_properties{ other.m_properties }
		, m_attached_source{ other.m_attached_source }
	{
		other.m_attached_source = nullptr;

		if (m_attached_source)
			m_attached_source->set_attached_sound(this);
	}

	sound_interface& sound_interface::operator = (const sound_interface& other)
	{
		m_properties = other.m_properties;

		return *this;
	}

	sound_interface& sound_interface::operator = (sound_interface&& other) noexcept
	{
		m_properties = other.m_properties;
		m_attached_source = other.m_attached_source;

		other.m_attached_source = nullptr;
		if (m_attached_source)
			m_attached_source->set_attached_sound(this);

		return *this;
	}

	void sound_interface::stop()
	{
		if (m_attached_source)
		{
			if (m_attached_source->get_looping())
			{
				audio_device::get().make_source_available(m_attached_source);
			}

			m_attached_source->stop();
		}
	}

	void sound_interface::update_source(sound_source& source, bool looped) const
	{
		source.set_position(m_properties.position);
		source.set_pitch(m_properties.pitch);
		source.set_volume(m_properties.volume);
		source.set_min_distance(m_properties.min_distance);
		source.set_attenuation(m_properties.attenuation);
		source.set_relative_to_listener(m_properties.relative_to_listener);
		if (audio_device::get().is_direct_channels_available()) source.set_direct_channels(m_properties.direct_channels);
		source.set_looping(looped);
	}

	void sound_interface::set_position(const glm::vec3& value)
	{
		m_properties.position = value;
	}

	void sound_interface::update_position(const glm::vec3& value)
	{
		if (m_properties.position != value)
		{
			set_position(value);
			if (m_attached_source) m_attached_source->set_position(value);
		}
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
		if (m_properties.pitch != value)
		{
			set_pitch(value);
			if (m_attached_source) m_attached_source->set_pitch(value);
		}
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
		if (m_properties.volume != value)
		{
			set_volume(value);
			if (m_attached_source) m_attached_source->set_volume(value);
		}
	}

	float sound_interface::get_volume() const
	{
		return m_properties.volume;
	}

	void sound_interface::set_min_distance(float value)
	{
		m_properties.min_distance = value;
	}

	void sound_interface::update_min_distance(float value)
	{
		if (m_properties.min_distance != value)
		{
			set_min_distance(value);
			if (m_attached_source) m_attached_source->set_min_distance(value);
		}
	}

	float sound_interface::get_min_distance() const
	{
		return m_properties.min_distance;
	}

	void sound_interface::set_attenuation(float value)
	{
		m_properties.attenuation = value;
	}

	void sound_interface::update_attenuation(float value)
	{
		if (m_properties.attenuation != value)
		{
			set_attenuation(value);
			if (m_attached_source) m_attached_source->set_attenuation(value);
		}
	}

	float sound_interface::get_attenuation() const
	{
		return m_properties.attenuation;
	}

	void sound_interface::set_relative_to_listener(bool value)
	{
		m_properties.relative_to_listener = value;
	}

	void sound_interface::update_relative_to_listener(bool value)
	{
		if (m_properties.relative_to_listener != value)
		{
			set_relative_to_listener(value);
			if (m_attached_source) m_attached_source->set_relative_to_listener(value);
		}
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
		if (m_properties.relative_to_listener != value)
		{
			set_direct_channels(value);
			if (m_attached_source && audio_device::get().is_direct_channels_available()) m_attached_source->set_direct_channels(value);
		}
	}

	bool sound_interface::get_direct_channels() const
	{
		return m_properties.direct_channels;
	}

	bool sound_interface::get_looping() const
	{
		if (m_attached_source)
			return m_attached_source->get_looping();

		return false;
	}

	void sound_interface::attach_source(sound_source* value)
	{
		if (m_attached_source == value) return;

		detach_source();
		m_attached_source = value;
		if (m_attached_source)
			m_attached_source->set_attached_sound(this);
	}

	sound_source* sound_interface::get_attached_source() const
	{
		return m_attached_source;
	}

	void sound_interface::detach_source() const
	{
		if (m_attached_source)
		{
			m_attached_source->set_attached_sound(nullptr);
			m_attached_source = nullptr;
		}
	}

	const sound_properties & sound_interface::get_properties() const
	{
		return m_properties;
	}
}

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
		, m_channel_link{ this, std::move(other.m_channel_link) }
	{}

	sound_interface& sound_interface::operator = (const sound_interface& other)
	{
		if (this == &other) return *this;

		m_properties = other.m_properties;

		return *this;
	}

	sound_interface& sound_interface::operator = (sound_interface&& other) noexcept
	{
		if (this == &other) return *this;

		m_properties = other.m_properties;

		//Call destructor of m_channel_link
		std::destroy_at(&m_channel_link);
		//Construct a new object with placement new into the memory address
		::new (&m_channel_link) channel_link{ this, std::move(other.m_channel_link) };

		return *this;
	}
}

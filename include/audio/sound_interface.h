#pragma once

#include <atomic>
#include <mutex>

#include <glm/vec3.hpp>
#include "properties.h"
#include "channel_link.h"

namespace age::audio
{
	class source;
	class buffer;
	class channel;

	class sound_interface
	{
	public:
		friend class audio_device;
		friend class source;
		friend class channel;

		sound_interface() = default;
		sound_interface(const sound_interface& other);
		sound_interface(sound_interface&& other) noexcept;

		sound_interface& operator = (const sound_interface& other);
		sound_interface& operator = (sound_interface&& other) noexcept;

		virtual ~sound_interface() = default;

	public:
		virtual void play(bool looped = false) = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void on_channel_lost() {}

		void set_position(const glm::vec3 value)
		{
			m_properties.position = value;
		}
		void update_position(const glm::vec3 value)
		{
			set_position(value);
			m_channel_link.set_position(value);
		}
		[[nodiscard]] glm::vec3 get_position() const
		{
			return m_properties.position;
		}

		void set_pitch(float value)
		{
			m_properties.pitch = value;
		}
		void update_pitch(float value)
		{
			set_pitch(value);
			m_channel_link.set_pitch(value);
		}
		[[nodiscard]] float get_pitch() const
		{
			return m_properties.pitch;
		}

		void set_volume(float value)
		{
			m_properties.volume = value;
		}
		void update_volume(float value)
		{
			set_volume(value);
			m_channel_link.set_volume(value);
		}
		[[nodiscard]] float get_volume() const
		{
			return m_properties.volume;
		}

		void set_reference_distance(float value)
		{
			m_properties.reference_distance = value;
		}
		void update_reference_distance(float value)
		{
			set_reference_distance(value);
			m_channel_link.set_reference_distance(value);
		}
		[[nodiscard]] float get_reference_distance() const
		{
			return m_properties.reference_distance;
		}

		void set_rolloff_factor(float value)
		{
			m_properties.rolloff_factor = value;
		}
		void update_rolloff_factor(float value)
		{
			set_rolloff_factor(value);
			m_channel_link.set_rolloff_factor(value);
		}
		[[nodiscard]] float get_rolloff_factor() const
		{
			return m_properties.rolloff_factor;
		}

		void set_relative_to_listener(bool value)
		{
			m_properties.relative_to_listener = value;
		}
		void update_relative_to_listener(bool value)
		{
			set_relative_to_listener(value);
			m_channel_link.set_relative_to_listener(value);
		}
		bool get_relative_to_listener() const
		{
			return m_properties.relative_to_listener;
		}

		void set_direct_channels(bool value)
		{
			m_properties.direct_channels = value;
		}
		void update_direct_channels(bool value)
		{
			set_direct_channels(value);
			m_channel_link.set_direct_channels(value);
		}
		bool get_direct_channels() const
		{
			return m_properties.direct_channels;
		}

		void set_auxiliary_bus(uint8_t value)
		{
			m_auxiliary_bus = value;
		}
		void update_auxiliary_bus(uint8_t value)
		{
			set_auxiliary_bus(value);
			m_channel_link.set_auxiliary_bus(value);
		}
		[[nodiscard]] uint8_t get_auxiliary_bus() const
		{
			return m_auxiliary_bus;
		}

		[[nodiscard]] bool get_looping() const
		{
			return m_properties.looping;
		}

	protected:
		[[nodiscard]] const channel_link& get_channel_link() const { return m_channel_link; }
		channel_link& get_channel_link() { return m_channel_link; }

		void attach_channel(const channel_guard& value) { m_channel_link.attach(value); }
		void attach_channel(reserved_channel&& chan) { m_channel_link.attach(std::move(chan)); }
		void detach_channel() { m_channel_link.detach(); }

		const properties& get_properties() const { return m_properties; }

	private:
		properties m_properties;

		channel_link m_channel_link{ this };

		uint8_t m_auxiliary_bus{};
	};
}
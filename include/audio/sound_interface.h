#pragma once

#include <glm/vec3.hpp>
#include "sound_properties.h"

namespace age
{
	class sound_source;
	class audio_channel;
	class sound_buffer;

	class sound_interface
	{
	public:
		friend class audio_device;
		friend class sound_source;
		friend class audio_channel;

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

		void update_source(sound_source& source, bool looped) const;

		//ToDo: Maybe add here set_position and update_position with a glm::vec2 too
		virtual void set_position(const glm::vec3& value);
		virtual void update_position(const glm::vec3& value);
		virtual const glm::vec3& get_position() const;

		virtual void set_pitch(float value);
		virtual void update_pitch(float value);
		virtual float get_pitch() const;

		virtual void set_volume(float value);
		virtual void update_volume(float value);
		virtual float get_volume() const;

		virtual void set_reference_distance(float value);
		virtual void update_reference_distance(float value);
		virtual float get_reference_distance() const;

		virtual void set_rolloff_factor(float value);
		virtual void update_rolloff_factor(float value);
		virtual float get_rolloff_factor() const;

		virtual void set_relative_to_listener(bool value);
		virtual void update_relative_to_listener(bool value);
		virtual bool get_relative_to_listener() const;

		virtual void set_direct_channels(bool value);
		virtual void update_direct_channels(bool value);
		virtual bool get_direct_channels() const;

		//virtual void set_looping(bool value);

		virtual bool get_looping() const;

	protected:
		void attach_channel(audio_channel* value);
		audio_channel* get_attached_channel() const;
		void detach_channel() const;

		const sound_properties& get_properties() const;

	private:
		sound_properties m_properties;

		mutable audio_channel* m_attached_channel{ nullptr };
	};
}
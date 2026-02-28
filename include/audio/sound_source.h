#pragma once
#include <glm/vec3.hpp>

#include "../utility/utility.h"
#include "sound_queue_buffer.h"
#include "sound_state.h"

namespace age
{
	class sound_interface;
	class sound_buffer;

	class sound_source
	{
	public:
		friend class audio_device;
		friend class sound;
		friend class sound_interface;

		class constructor_key { friend class audio_device; constructor_key() {} };

		sound_source(constructor_key, uint32_t handle);

		sound_source(const sound_source& other) = delete;
		sound_source(sound_source&& other) noexcept = default;

		sound_source& operator = (const sound_source& other) = delete;
		sound_source& operator = (sound_source&& other) noexcept = default;

	public:
		void play();
		void stop();
		void pause();
		void rewind();
		
		void set_position(const glm::vec3& value);
		glm::vec3 get_position() const;

		void set_pitch(float value);
		float get_pitch() const;

		void set_volume(float value);
		float get_volume() const;

		void set_min_distance(float value);
		float get_min_distance() const;

		void set_attenuation(float value);
		float get_attenuation() const;

		void set_relative_to_listener(bool value);
		bool get_relative_to_listener() const;

		void set_direct_channels(bool value);
		bool get_direct_channels() const;

		void set_looping(bool value);
		bool get_looping() const;

		void set_buffer(const sound_buffer& value);
		bool has_buffer_attached(const sound_buffer& value) const;

		void detach_buffer(const sound_buffer& value);

		void queue_buffer(sound_queue_buffer value);
		uint32_t get_num_queued_buffers() const;
		uint32_t get_num_processed_buffers() const;
		sound_queue_buffer unqueue_buffer();

		void clear_buffers();

		void invalidate();
		
		sound_state get_state() const;

	protected:

	private:
		sound_source();
		sound_source(uint32_t handle);

		bool ensure_handle() const;

		void set_attached_sound(sound_interface* value);
		sound_interface* get_attached_sound() const;

		void enable_source_spatialize();

		void detach_sound();

		sound_interface* m_attached_sound;
		static uint32_t gen_handle();
		static void delete_handle(uint32_t handle);
		mutable unique_handle<uint32_t, delete_handle> m_handle;

		uint32_t m_attached_buffer;
	};
}
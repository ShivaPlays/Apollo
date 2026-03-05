#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <gch/small_vector.hpp>

#include "../utility/utility.h"
#include "sound_queue_buffer.h"
#include "sound_state.h"
#include "sound_properties.h"

namespace age
{
	class sound_buffer;

	class sound_source
	{
	public:
		friend class audio_device;

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

		void apply_properties(const sound_properties& properties, bool force = false);
		
		void set_position(const glm::vec3& value);
		const glm::vec3& get_position() const;

		void set_velocity(const glm::vec3& value);
		const glm::vec3& get_velocity() const;

		void set_direction(const glm::vec3& value);
		const glm::vec3& get_direction() const;

		void set_radius(float value);
		float get_radius() const;

		void set_cone_inner_angle(float value);
		float get_cone_inner_angle() const;

		void set_cone_outer_angle(float value);
		float get_cone_outer_angle() const;

		void set_cone_outer_gain(float value);
		float get_cone_outer_gain() const;

		void set_pitch(float value);
		float get_pitch() const;

		void set_volume(float value);
		float get_volume() const;

		void set_min_gain(float value);
		float get_min_gain() const;

		void set_max_gain(float value);
		float get_max_gain() const;

		void set_reference_distance(float value);
		float get_reference_distance() const;

		void set_max_distance(float value);
		float get_max_distance() const;

		void set_rolloff_factor(float value);
		float get_rolloff_factor() const;

		void set_relative_to_listener(bool value);
		bool get_relative_to_listener() const;

		void set_air_absorption_factor(float value);
		float get_air_absorption_factor() const;

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

		bool ensure_handle();
		void enable_source_spatialize();

		static uint32_t gen_handle();
		static void delete_handle(uint32_t handle);

		sound_properties m_properties;

		mutable unique_handle<uint32_t, delete_handle> m_handle;
		gch::small_vector<uint32_t, 16> m_queued_buffers;
		uint32_t m_attached_buffer;
	};
}
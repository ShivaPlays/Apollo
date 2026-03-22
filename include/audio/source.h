#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <gch/small_vector.hpp>

#include "../utility/utility.h"
#include "queue_buffer.h"
#include "state.h"
#include "properties.h"
#include "config.h"
#include "filter.h"

namespace age::audio::effect
{
	class slot;
	class auxiliary_send_group;
}

namespace age::audio
{
	class buffer;
	class group;

	class source
	{
	public:
		friend class device;

		class constructor_key { friend class device; constructor_key() {} };

		source(constructor_key, uint32_t handle);

		source(const source& other) = delete;
		source(source&& other) noexcept = default;

		source& operator = (const source& other) = delete;
		source& operator = (source&& other) noexcept = default;

	public:
		void play();
		void stop();
		void pause();
		void rewind();

		void apply_properties(const properties& properties, bool force = false);
		
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

		void attach_buffer(const buffer& value);
		bool has_buffer_attached(const buffer& value) const;
		void detach_buffer(const buffer& value);

		void enqueue_buffer(queue_buffer value);
		uint32_t get_num_queued_buffers() const;
		uint32_t get_num_processed_buffers() const;
		queue_buffer unqueue_buffer();

		void clear_buffers();

		void set_effect_slot(size_t index, const effect::slot& slot);
		void update_effect_slots(const effect::auxiliary_send_group& group);
		void reset_effect_slots();

		void invalidate();
		
		state get_state() const;

	protected:

	private:
		source();
		source(uint32_t handle);

		bool realize();
		void enable_source_spatialize();

		static uint32_t gen_handle();
		static void delete_handle(uint32_t handle);

		properties m_properties;

		filter m_direct_filter;
		std::array<filter, config::MAX_AUXILIARY_SENDS> m_send_filters{};

		mutable unique_handle<uint32_t, delete_handle> m_handle{};
		gch::small_vector<uint32_t, 16> m_queued_buffers{};
		std::array<uint32_t, config::MAX_AUXILIARY_SENDS> m_slots{};

		uint32_t m_attached_buffer{};
	};
}
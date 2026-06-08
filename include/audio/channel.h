//
// Created by skaldi on 03.03.26.
//
#pragma once

#include <mutex>
#include <atomic>

#include "source.h"
//#include "channel_link.h"
#include "properties.h"

namespace age::audio
{
    class channel_link;

    class channel
    {
        friend class device;
        friend class channel_guard;
        friend class channel_link;

    public:
        channel(source::constructor_key key, uint32_t handle)
            : m_source{ key, handle }
        {}

        // We must manually define how to move this class
        channel(channel&& other) noexcept
            : m_source{ std::move(other.m_source) }
            , m_owner{ other.m_owner }
            , m_busy{ other.m_busy.load() }
            , m_priority{ other.m_priority }
            , m_is_reserved{ other.m_is_reserved }
        {}

        channel& operator=(channel&&) = delete;
        channel(const channel&) = delete;
        channel& operator=(const channel&) = delete;

    public:
        void play() { m_source.play(); }
        void stop() { m_source.stop(); }
        void pause() { m_source.pause(); }
        void rewind() { m_source.rewind(); }

        void apply_properties(const properties& properties) { m_source.apply_properties(properties); }

        void set_position(const glm::vec3& value) { m_source.set_position(value); }
        const glm::vec3& get_position() const { return m_source.get_position(); }

        void set_velocity(const glm::vec3& value) { m_source.set_velocity(value); }
        const glm::vec3& get_velocity() const { return m_source.get_velocity(); }

        void set_direction(const glm::vec3& value) { m_source.set_direction(value); }
        const glm::vec3& get_direction() const { return m_source.get_direction(); }

        void set_radius(float value) { m_source.set_radius(value); }
        float get_radius() const { return m_source.get_radius(); }

        void set_cone_inner_angle(float value) { m_source.set_cone_inner_angle(value); }
        float get_cone_inner_angle() const { return m_source.get_cone_inner_angle(); }

        void set_cone_outer_angle(float value) { m_source.set_cone_outer_angle(value); }
        float get_cone_outer_angle() const { return m_source.get_cone_outer_angle(); }

        void set_cone_outer_gain(float value) { m_source.set_cone_outer_gain(value); }
        float get_cone_outer_gain() const { return m_source.get_cone_outer_gain(); }

        void set_pitch(float value) { m_source.set_pitch(value); }
        float get_pitch() const { return m_source.get_pitch(); }

        void set_volume(float value) { m_source.set_volume(value); }
        float get_volume() const { return m_source.get_volume(); };

        void set_min_gain(float value) { m_source.set_min_gain(value); }
        float get_min_gain() const { return m_source.get_min_gain(); }

        void set_max_gain(float value) { m_source.set_max_gain(value); }
        float get_max_gain() const { return m_source.get_max_gain(); }

        void set_reference_distance(float value) { m_source.set_reference_distance(value); }
        float get_reference_distance() const { return m_source.get_reference_distance(); }

        void set_max_distance(float value) { m_source.set_max_distance(value); }
        float get_max_distance() const { return m_source.get_max_distance(); }

        void set_rolloff_factor(float value) { m_source.set_rolloff_factor(value); }
        float get_rolloff_factor() const { return m_source.get_rolloff_factor(); }

        void set_relative_to_listener(bool value) { m_source.set_relative_to_listener(value); }
        bool get_relative_to_listener() const { return m_source.get_relative_to_listener(); }

        void set_air_absorption_factor(float value) { m_source.set_air_absorption_factor(value); }
        float get_air_absorption_factor() const { return m_source.get_air_absorption_factor(); }

        void set_direct_channels(bool value) { m_source.set_direct_channels(value); }
        bool get_direct_channels() const { return m_source.get_direct_channels(); }

        void set_looping(bool value) { m_source.set_looping(value); }
        bool get_looping() const { return m_source.get_looping(); }

        void attach_buffer(const buffer& value) { m_source.attach_buffer(value); }
        bool has_buffer_attached(const buffer& value) const { return m_source.has_buffer_attached(value); }
        void detach_buffer(const buffer& value) { m_source.detach_buffer(value); }

        void enqueue_buffer(queue_buffer value) { m_source.enqueue_buffer(value); }
        uint32_t get_num_queued_buffers() const { return m_source.get_num_queued_buffers(); }
        uint32_t get_num_processed_buffers() const { return m_source.get_num_processed_buffers(); }
        queue_buffer unqueue_buffer() { return m_source.unqueue_buffer(); }

        void clear_buffers() { m_source.clear_buffers(); }

        void invalidate() { m_source.invalidate(); }

        state get_state() const { return m_source.get_state(); }

        void set_reserved(bool value) { m_is_reserved = value; }
        bool is_reserved() const { return m_is_reserved; }

        void set_priority(uint8_t value) { m_priority = value; }
        uint8_t get_priority() const { return m_priority; }

        std::mutex& get_state_mutex() const { return m_state_mutex; }

        void set_owner(channel_link* owner)
        {
            std::scoped_lock lock{ m_owner_mutex };
            m_owner = owner;
        }

        void set_auxiliary_bus(uint8_t value) { m_auxiliary_bus = value; apply_auxiliary_bus(); }
        uint8_t get_auxiliary_bus() const { return m_auxiliary_bus; }
        void apply_auxiliary_bus();

        bool is_free() const { return !m_is_reserved && m_source.get_state() == state::stopped; }

    protected:

    private:
        source& get_source () { return m_source; }
        const source& get_source() const { return m_source; }

        void release_owner(const channel_link& expected_owner)
        {
            std::scoped_lock lock{ m_owner_mutex };

            if (m_owner == &expected_owner)
            {
                m_owner = nullptr;
            }
        }

        bool try_acquire() { return !m_busy.exchange(true); }
        void release() { m_busy.store(false); }
        bool is_busy() const { return m_busy.load(); }

        source m_source;

        mutable std::mutex m_state_mutex;
        mutable std::mutex m_owner_mutex;
        channel_link* m_owner{};

        std::atomic<bool> m_busy{false};

        uint8_t m_auxiliary_bus{};

        uint8_t m_priority{};
        bool m_is_reserved{};
    };
}
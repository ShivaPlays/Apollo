//
// Created by skaldi on 03.03.26.
//
#pragma once

#include <atomic>

#include "source.h"
#include "sound_interface.h"
#include "properties.h"

namespace age::audio
{
    class channel
    {
        friend class device;
        friend class sound_interface;
        friend class channel_guard;

    public:
        channel(source::constructor_key key, uint32_t handle)
            : m_source{ key, handle }
        {}

        // We must manually define how to move this class
        channel(channel&& other) noexcept
            : m_source(std::move(other.m_source)),
              m_owner(other.m_owner),
              m_priority(other.m_priority),
              m_is_reserved(other.m_is_reserved)
        {
            m_busy.store(other.m_busy.load());
        }

        channel& operator=(channel&&) = delete;
        channel(const channel&) = delete;
        channel& operator=(const channel&) = delete;

        source& get_source () { return m_source; }
        const source& get_source() const { return m_source; }

        void apply_properties(const properties& properties) { m_source.apply_properties(properties); }

        void set_reserved(bool value) { m_is_reserved = value; }
        bool is_reserved() const { return m_is_reserved; }

        void set_priority(uint8_t value) { m_priority = value; }
        uint8_t get_priority() const { return m_priority; }

        void set_owner(sound_interface* owner) { m_owner = owner; }
        sound_interface* get_owner() const { return m_owner; }

        void set_filter_group(uint16_t value) { m_filter_group = value; }
        uint16_t get_filter_group() const { return m_filter_group; }

    public:
        bool is_free() const { return !m_is_reserved && m_source.get_state() == state::stopped; }

    protected:

    private:
        void detach_owner()
        {
            if (m_owner)
            {
                m_owner->attach_channel(nullptr);
                m_owner = nullptr;
            }
        }

        bool try_acquire() { return !m_busy.exchange(true); }
        void release() { m_busy.store(false); }
        bool is_busy() const { return m_busy.load(); }

        source m_source;
        sound_interface* m_owner{};

        uint16_t m_filter_group{};

        std::atomic<bool> m_busy{false};

        uint8_t m_priority{};
        bool m_is_reserved{};
    };
}
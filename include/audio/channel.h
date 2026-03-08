//
// Created by skaldi on 03.03.26.
//
#pragma once

#include <mutex>
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
            : m_source{ std::move(other.m_source) }
            , m_owner{ other.m_owner.load() }
            , m_busy{ other.m_busy.load() }
            , m_priority{ other.m_priority }
            , m_is_reserved{ other.m_is_reserved }
        {}

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

        std::mutex& get_state_mutex() const { return m_state_mutex; }
        std::mutex& get_owner_mutex() const { return m_owner_mutex; }

        void set_owner(sound_interface* owner)
        {
            std::lock_guard lock{ m_owner_mutex };
            set_owner_locked(owner);
        }
        sound_interface* get_owner() const { return m_owner.load(std::memory_order_acquire); }

        void set_filter_group(uint16_t value) { m_filter_group = value; }
        uint16_t get_filter_group() const { return m_filter_group; }

    public:
        bool is_free() const { return !m_is_reserved && m_source.get_state() == state::stopped; }

    protected:

    private:
        void set_owner_locked(sound_interface* owner) { m_owner.store(owner, std::memory_order_release); }

        void detach_owner()
        {
            std::lock_guard lock{ m_owner_mutex };

            if (auto owner = m_owner.load(std::memory_order_relaxed))
            {
                owner->attach_channel(nullptr);
                m_owner.store(nullptr, std::memory_order_release);
            }
        }

        bool try_acquire() { return !m_busy.exchange(true); }
        void release() { m_busy.store(false); }
        bool is_busy() const { return m_busy.load(); }

        source m_source;

        mutable std::mutex m_state_mutex;
        mutable std::mutex m_owner_mutex;
        std::atomic<sound_interface*> m_owner{};

        uint16_t m_filter_group{};

        std::atomic<bool> m_busy{false};

        uint8_t m_priority{};
        bool m_is_reserved{};
    };
}
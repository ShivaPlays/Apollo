//
// Created by skaldi on 03.03.26.
//

#pragma once

#include <mutex>

#include "channel.h"

namespace age::audio
{
    class channel_guard
    {
    public:
        friend class device;

        channel_guard(const channel_guard&) = delete;
        channel_guard& operator=(const channel_guard&) = delete;

        channel_guard(channel_guard&& other) noexcept
            : m_channel_lock{ std::move(other.m_channel_lock) }
            , m_channel(std::exchange(other.m_channel, nullptr))
        {}

        channel_guard& operator=(channel_guard&& other) noexcept
        {
            if (this != &other)
            {
                release();
                m_channel_lock = std::move(other.m_channel_lock);
                m_channel = std::exchange(other.m_channel, nullptr);
            }

            return *this;
        }

        ~channel_guard() { release(); }

        channel* get() const { return m_channel; }
        channel* operator->() const { return m_channel; }
        channel& operator*() const { return *m_channel; }
        explicit operator bool() const { return m_channel != nullptr; }
    public:

    protected:

    private:
        static std::unique_lock<std::mutex> create_lock(channel* c)
        {
            if (c) return std::unique_lock{ c->get_state_mutex() };
            return std::unique_lock<std::mutex>{};
        }

        channel_guard(channel* channel) noexcept
            : m_channel_lock{ create_lock(channel) }
            , m_channel{ channel }
        {}

        channel_guard(std::unique_lock<std::mutex>&& lock, channel& channel) noexcept
            : m_channel_lock{ std::move(lock) }
            , m_channel{ &channel }
        {}

        void release()
        {
            if (m_channel)
            {
                if (m_channel_lock.owns_lock()) m_channel_lock.unlock();
                m_channel = nullptr;
            }
        }

        std::unique_lock<std::mutex> m_channel_lock;
        channel* m_channel;
    };
}
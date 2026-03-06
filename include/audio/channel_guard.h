//
// Created by skaldi on 03.03.26.
//

#pragma once

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
            : m_channel(std::exchange(other.m_channel, nullptr))
        {}

        channel_guard& operator=(channel_guard&& other) noexcept
        {
            if (this != &other)
            {
                release();
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
        channel_guard(channel* channel) { m_channel = channel; }

        void release()
        {
            if (m_channel)
            {
                m_channel->release();
                m_channel = nullptr;
            }
        }

        channel* m_channel{ nullptr };
    };
}
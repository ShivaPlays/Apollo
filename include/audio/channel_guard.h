//
// Created by skaldi on 03.03.26.
//

#pragma once

#include "audio_channel.h"

namespace age
{
    class channel_guard
    {
    public:
        friend class audio_device;

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

        audio_channel* get() const { return m_channel; }
        audio_channel* operator->() const { return m_channel; }
        audio_channel& operator*() const { return *m_channel; }
        explicit operator bool() const { return m_channel != nullptr; }
    public:

    protected:

    private:
        channel_guard(audio_channel* channel) { m_channel = channel; }

        void release()
        {
            if (m_channel)
            {
                m_channel->release();
                m_channel = nullptr;
            }
        }

        audio_channel* m_channel{ nullptr };
    };
}
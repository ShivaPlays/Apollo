//
// Created by skaldi on 08.06.26.
//

#pragma once

#include "channel.h"
#include "channel_guard.h"

namespace age::audio
{
    class reserved_channel
    {
    public:
        explicit reserved_channel(const channel_guard &chan)
            : m_channel(chan.get())
        {
            if (m_channel) m_channel->set_reserved(true);
        }

        reserved_channel(reserved_channel&& other) noexcept
           : m_channel(std::exchange(other.m_channel, nullptr))
        {}

        reserved_channel& operator=(reserved_channel&& other) noexcept
        {
            if (this == &other) return *this;

            if (m_channel) m_channel->set_reserved(false);

            m_channel = std::exchange(other.m_channel, nullptr);
            return *this;
        }

        reserved_channel(const reserved_channel& other) = delete;
        reserved_channel& operator=(const reserved_channel& other) = delete;

        ~reserved_channel()
        {
            if (m_channel) m_channel->set_reserved(false);
        }

    public:
        channel* get() const { return m_channel; }
    protected:

    private:
        channel* m_channel{};
    };
}
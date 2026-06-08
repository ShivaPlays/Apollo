//
// Created by skaldi on 08.06.26.
//

#pragma once

#include <mutex>
#include <variant>

#include <glm/vec3.hpp>

#include "channel.h"
#include "channel_guard.h"
#include "reserved_channel.h"

namespace age::audio
{
    class channel_guard;
    class sound_interface;

    class channel_link
    {
    public:
        channel_link(sound_interface* owner)
            : m_owner(owner)
        {}

        channel_link(channel_link&& other) noexcept
            : m_owner(other.m_owner)
            , m_is_valid{ other.m_is_valid.load(std::memory_order_relaxed) }
        {
            // Lock both instances safely to move the hardware tracking state
            std::scoped_lock lock{ m_mutex, other.m_mutex };

            m_state = std::move(other.m_state);

            other.m_state = nullptr; // Reset the old one back to an idle state
            other.m_is_valid.store(false, std::memory_order_relaxed);

            // If the moved state was actively holding a channel, update its back-pointer
            if (auto chan = get_raw_pointer()) chan->set_owner(this);
        }

        channel_link& operator=(channel_link&& other) noexcept
        {
            if (this == &other) return *this;

            // Lock both instances safely
            std::scoped_lock lock{ m_mutex, other.m_mutex };

            if (auto chan = get_raw_pointer()) chan->release_owner(*this);

            m_state = std::move(other.m_state);
            m_is_valid.store(other.m_is_valid.load(std::memory_order_relaxed), std::memory_order_relaxed);

            other.m_state = nullptr;
            other.m_is_valid.store(false, std::memory_order_relaxed);

            if (channel* chan = get_raw_pointer()) chan->set_owner(this);

            return *this;
        }

        ~channel_link()
        {
            detach();
        }

    public:
        template <typename F>
        void execute_on_channel(F&& func)
        {
            std::scoped_lock lock{ m_mutex };
            if (channel* chan = get_raw_pointer())
            {
                std::forward<F>(func)(*chan);
            }
        }

        void play()
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->play();
        }

        void stop()
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->stop();
        }

        void pause()
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->pause();
        }

        void rewind()
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->rewind();
        }

        void apply_properties(const properties& properties)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->apply_properties(properties);
        }

        void set_position(glm::vec3 value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_position(value);
        }

        void set_velocity(glm::vec3 value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_velocity(value);
        }

        void set_direction(glm::vec3 value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_direction(value);
        }

        void set_radius(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_radius(value);
        }

        void set_cone_inner_angle(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_cone_inner_angle(value);
        }

        void set_cone_outer_angle(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_cone_outer_angle(value);
        }

        void set_cone_outer_gain(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_cone_outer_gain(value);
        }

        void set_pitch(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_pitch(value);
        }

        void set_volume(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_volume(value);
        }

        void set_min_gain(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_min_gain(value);
        }

        void set_max_gain(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_max_gain(value);
        }

        void set_reference_distance(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_reference_distance(value);
        }

        void set_max_distance(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_max_distance(value);
        }

        void set_rolloff_factor(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_rolloff_factor(value);
        }

        void set_relative_to_listener(bool value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_relative_to_listener(value);
        }

        void set_air_absorption_factor(float value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_air_absorption_factor(value);
        }

        void set_direct_channels(bool value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_direct_channels(value);
        }

        void set_looping(bool value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_looping(value);
        }

        void set_auxiliary_bus(uint8_t value)
        {
            std::scoped_lock lock{ m_mutex };
            if (auto chan = get_raw_pointer()) chan->set_auxiliary_bus(value);
        }

        void attach(const channel_guard& chan)
        {
            std::scoped_lock lock{m_mutex };

            auto new_channel = chan.get();
            auto old_channel = get_raw_pointer();

            m_state = new_channel;
            m_is_valid.store(new_channel != nullptr, std::memory_order_relaxed);

            release_assign_owner(old_channel, new_channel);
        }

        void attach(reserved_channel&& chan)
        {
            std::scoped_lock lock{m_mutex };

            auto new_channel = chan.get();
            auto old_channel = get_raw_pointer();

            m_state = std::move(chan);
            m_is_valid.store(new_channel != nullptr, std::memory_order_relaxed);

            release_assign_owner(old_channel, new_channel);
        }

        void detach()
        {
            std::scoped_lock lock{m_mutex };

            auto current_channel = get_raw_pointer();
            m_state = nullptr;
            m_is_valid.store(false, std::memory_order_relaxed);

            if (current_channel) current_channel->release_owner(*this);
        }

        [[nodiscard]] bool is_valid() const
        {
            return m_is_valid.load(std::memory_order_relaxed);
        }

    protected:

    private:
        channel *get_raw_pointer() const
        {
            return std::visit([](auto&& arg) -> channel* {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, channel*>) {
                    return arg;
                } else {
                    return arg.get(); // reserved_channel handles extraction
                }
            }, m_state);
        }

        void release_assign_owner(channel* old_channel, channel* new_channel)
        {
            if (old_channel == new_channel) return;

            if (old_channel) old_channel->release_owner(*this);
            if (new_channel) new_channel->set_owner(this);
        }

        std::mutex m_mutex;
        std::variant<channel*, reserved_channel> m_state{ nullptr };
        sound_interface* m_owner{};

        std::atomic<bool> m_is_valid{ false };
    };
}

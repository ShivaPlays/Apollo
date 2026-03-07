//
// Created by skaldi on 07.03.26.
//

#pragma once

#include <mutex>
#include <algorithm>
#include <gch/small_vector.hpp>

namespace age::core
{
    template <typename T, size_t N>
    class dependency_tracker
    {
    public:
        // Move constructor: Lock other, steal data
        dependency_tracker(dependency_tracker&& other) noexcept
        {
            std::lock_guard lock{ other.m_mutex };
            m_dependents = std::move(other.m_dependents);
        }

        // Move assignment: Lock both, steal data
        dependency_tracker& operator=(dependency_tracker&& other) noexcept
        {
            if (this != &other)
            {
                std::scoped_lock lock{ m_mutex, other.m_mutex };
                m_dependents = std::move(other.m_dependents);
            }
            return *this;
        }

    public:
        // Adds a new dependent to the list
        void add(T* value)
        {
            std::lock_guard lock{ m_mutex };
            m_dependents.push_back(value);
        }

        // Removes a specific dependent (e.g. when a slot attaches to a different effect)
        void remove(T* value)
        {
            std::lock_guard lock{ m_mutex };
            auto it = std::remove(m_dependents.begin(), m_dependents.end(), value);
            m_dependents.erase(it, m_dependents.end());
        }

        // Handles the "Move" logic safely
        void update_address(T* old_addr, T* new_addr)
        {
            std::lock_guard lock{ m_mutex };
            if (auto it = std::find(m_dependents.begin(), m_dependents.end(), old_addr);
                it != m_dependents.end())
            {
                *it = new_addr;
            }
            else
            {
                m_dependents.push_back(new_addr);
            }
        }

        // Notifies everyone and clears the list
        template <typename Func>
        void notify_and_clear(Func&& callback)
        {
            std::lock_guard lock{ m_mutex };
            for (auto* dependent : m_dependents)
            {
                callback(dependent);
            }
            m_dependents.clear();
        }

    protected:

    private:
        std::mutex m_mutex;
        gch::small_vector<T*, N> m_dependents;
    };
};

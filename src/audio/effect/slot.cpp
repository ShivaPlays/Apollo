//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/slot.h"

#include "audio/effect/effect_interface.h"
#include "audio/filter.h"

namespace age::audio::effect
{
    slot::~slot()
    {
        notify_death();
    }

    slot::slot(slot&& other) noexcept
    {
        std::scoped_lock lock(m_effect_mutex, other.m_effect_mutex);
        m_effect = std::exchange(other.m_effect, nullptr);
        if (m_effect)
        {
            // Tell the effect to swap the address in the registry
            m_effect->update_slot_address(&other, this);
        }
    }

    slot& slot::operator=(slot&& other) noexcept
    {
        if (this == &other) return *this;

        std::scoped_lock lock(m_effect_mutex, other.m_effect_mutex);

        if (m_effect) m_effect->remove_slot(this);

        // Transfer the effect
        m_effect = std::exchange(other.m_effect, nullptr);

        if (m_effect)
        {
            // Tell the effect that the pointer has changed from 'other' to 'this'
            // This is safer than removing and adding, because it avoids
            // triggering a "destroyed" signal for a slot that is just moving.
            m_effect->update_slot_address(&other, this);
        }

        return *this;
    }

    void slot::notify_death()
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect) m_effect->remove_slot(this);
    }

    void slot::attach_effect(effect_interface *value)
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect) m_effect->remove_slot(this);

        m_effect = value;

        if (m_effect) m_effect->register_slot(this);
        apply_effect();
    }

    void slot::attach_filter(filter* value)
    {
        std::lock_guard lock{ m_filter_mutex };

        if (m_filter) m_filter->remove_slot(this);

        m_filter = value;

        if (m_filter) m_filter->register_slot(this);

       apply_filter();
    }

    void slot::apply_effect()
    {
        if (m_effect)
        {
            //ToDo: Apply effect
        }
        else
        {
            //ToDo: Apply NULL effect
        }
    }

    void slot::apply_filter()
    {
        if (m_filter)
        {
            //ToDo: Apply filter
        }
        else
        {
            //ToDo: Apply NULL filter
        }
    }

    void slot::on_effect_destroyed(effect_interface* value)
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect == value)
        {
            m_effect = nullptr;
            apply_effect();
        }
    }

    void slot::on_filter_destroyed(filter *value)
    {
        std::lock_guard lock{ m_filter_mutex };

        if (m_filter == value)
        {
            m_filter = nullptr;
            apply_filter();
        }
    }
}

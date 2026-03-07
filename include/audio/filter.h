//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "../core/dependency_tracker.h"

namespace age::audio::effect
{
    class slot;
}

namespace age::audio
{
    class filter final
    {
    public:
        friend class effect::slot;

        ~filter() { notify_death(); }

        filter(const filter&) = delete;
        filter& operator=(const filter&) = delete;

        filter(filter&& other) noexcept = default;
        filter& operator=(filter&& other) noexcept = default;

    public:

    protected:

    private:
        void register_slot(effect::slot* value) { m_tracker.add(value); }
        void remove_slot(effect::slot* value)   { m_tracker.remove(value); }
        void update_slot_address(effect::slot* old_addr, effect::slot* new_addr) { m_tracker.update_address(old_addr, new_addr); }

        void notify_death();

        core::dependency_tracker<effect::slot, 8> m_tracker;
    };
}

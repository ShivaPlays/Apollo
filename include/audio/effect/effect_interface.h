//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "../../core/dependency_tracker.h"

namespace age::audio::effect
{
    class slot;

    class effect_interface
    {
    public:
        friend class slot;

        effect_interface(const effect_interface&) = delete;
        effect_interface& operator=(const effect_interface&) = delete;

        effect_interface(effect_interface&& other) noexcept = default;
        effect_interface& operator=(effect_interface&& other) noexcept = default;

        virtual ~effect_interface() { notify_death(); }

    public:
        void register_slot(slot* value) { m_tracker.add(value); }

    protected:

    private:
        void notify_death();
        void remove_slot(slot* value) { m_tracker.remove(value); }
        void update_slot_address(slot* old_addr, slot* new_addr) { m_tracker.update_address(old_addr, new_addr); }

        core::dependency_tracker<slot, 8> m_tracker;
    };
}
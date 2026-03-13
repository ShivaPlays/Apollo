//
// Created by skaldi on 07.03.26.
//

#pragma once

#include <array>

#include "slot.h"
#include "../config.h"
#include "../../utility/make_array.h"

namespace age::audio::effect
{
    class auxiliary_send_group
    {
        friend class slot;
    public:

    public:
        void on_slot_filter_changed(const slot& s);
        std::array<slot, config::MAX_AUXILIARY_SENDS>& get_slots() { return m_slots; }
        const std::array<slot, config::MAX_AUXILIARY_SENDS>& get_slots() const { return m_slots; }

        void set_enabled(bool value) { m_enabled = value; }
        bool get_enabled() const { return m_enabled; }

    protected:

    private:
        std::array<slot, config::MAX_AUXILIARY_SENDS> m_slots{ utility::make_array_factory<slot, config::MAX_AUXILIARY_SENDS>([this](std::size_t i) { return slot(this, i); }) };

        bool m_enabled = false;
    };
}
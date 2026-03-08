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
    class group
    {
        friend class slot;
    public:

    public:
        void on_slot_filter_changed(const slot& s);

    protected:

    private:
        std::array<slot, config::MAX_AUXILIARY_SENDS> m_slots{ utility::make_array<slot, config::MAX_AUXILIARY_SENDS>(this) };
    };
}
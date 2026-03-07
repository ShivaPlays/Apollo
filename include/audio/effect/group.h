//
// Created by skaldi on 07.03.26.
//

#pragma once

#include <array>

#include "slot.h"
#include "../config.h"

namespace age::audio::effect
{
    class group
    {
    public:

    public:

    protected:

    private:
        std::array<slot, config::MAX_AUXILIARY_SENDS> m_slots{};
    };
}
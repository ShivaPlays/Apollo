//
// Created by skaldi on 06.03.26.
//

#pragma once


#include "audio/effect/effect_interface.h"

namespace age::audio::effect
{
    class pitch_shifter : public effect_interface
    {
    public:

    public:
        void set_coarse_tune(int32_t value);
        int32_t get_coarse_tune() const;

        void set_fine_tune(int32_t value);
        int32_t get_fine_tune() const;

    protected:
        void init() override;

    private:
        int32_t m_coarse_tune{ 12 };
        int32_t m_fine_tune{ 0 };
    };
}
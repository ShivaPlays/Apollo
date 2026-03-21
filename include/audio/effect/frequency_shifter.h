//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class frequency_shifter : public effect_interface
    {
    public:
        enum class direction
        {
            down,
            up,
            off,

            count
        };

    public:
        void set_frequency(float value);
        float get_frequency() const;

        void set_left_direction(direction value);
        direction get_left_direction() const;

        void set_right_direction(direction value);
        direction get_right_direction() const;

    protected:
        void init() const override;

    private:
        float m_frequency{ 0.0f };
        int32_t m_left_direction{ 0 };
        int32_t m_right_direction{ 0 };
    };
}
//
// Created by skaldi on 21.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class dedicated_dialog : public effect_interface
    {
    public:

    public:
        void set_gain(float value);
        float get_gain() const;


    protected:
        void init() const override;

    private:
        float m_gain{ 1.0f };
    };
}
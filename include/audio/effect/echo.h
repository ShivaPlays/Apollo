//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "audio/effect/effect_interface.h"

namespace age::audio::effect
{
    class echo : public effect_interface
    {
    public:

    public:
        void set_delay(float value);
        float get_delay() const;

        void set_lr_delay(float value);
        float get_lr_delay() const;

        void set_feedback(float value);
        float get_feedback() const;

        void set_damping(float value);
        float get_damping() const;

        void set_spread(float value);
        float get_spread() const;

    protected:
        void init() override;

    private:
        float m_delay{ 0.1f };
        float m_lr_delay{ 0.1f };

        float m_feedback{ 0.5f };
        float m_damping{ 0.5f };

        float m_spread{ -1.0f };
    };
}
//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class autowah : public effect_interface
    {
    public:

    public:
        void set_attack_time(float value);
        float get_attack_time() const;

        void set_release_time(float value);
        float get_release_time() const;

        void set_resonance(float value);
        float get_resonance() const;

        void set_peak_gain(float value);
        float get_peak_gain() const;

    protected:
        void init() const override;
    private:

        float m_attack_time{ 0.06f };
        float m_release_time{ 0.06f };
        float m_resonance{ 1000.0f };
        float m_peak_gain{ 11.22f };
    };
}
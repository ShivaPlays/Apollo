//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class distortion : public effect_interface
    {
    public:

    public:
        void set_edge(float value);
        float get_edge() const;

        void set_gain(float value);
        float get_gain() const;

        void set_lowpass_cutoff(float value);
        float get_lowpass_cutoff() const;

        void set_eq_center(float value);
        float get_eq_center() const;

        void set_eq_bandwidth(float value);
        float get_eq_bandwidth() const;

    protected:
        void init() const override;

    private:
        float m_edge{ 0.2f };
        float m_gain{ 0.05f };
        float m_lowpass_cutoff{ 8000.0f };
        float m_eq_center{ 3600.0f };
        float m_eq_bandwidth{ 3600.0f };
    };
}
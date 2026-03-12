//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "audio/effect/effect_interface.h"

namespace age::audio::effect
{
    class equalizer : public effect_interface
    {
    public:

    public:
        void set_low_gain(float value);
        float get_low_gain() const;

        void set_low_cutoff(float value);
        float get_low_cutoff() const;

        void set_mid1_gain(float value);
        float get_mid1_gain() const;

        void set_mid1_center(float value);
        float get_mid1_center() const;

        void set_mid1_width(float value);
        float get_mid1_width() const;

        void set_mid2_gain(float value);
        float get_mid2_gain() const;

        void set_mid2_center(float value);
        float get_mid2_center() const;

        void set_mid2_width(float value);
        float get_mid2_width() const;

        void set_high_gain(float value);
        float get_high_gain() const;

        void set_high_cutoff(float value);
        float get_high_cutoff() const;

    protected:
        void init() override;

    private:
        float m_low_gain{ 1.0f };
        float m_low_cutoff{ 200.0f };

        float m_mid1_gain{ 1.0f };
        float m_mid1_center { 500.0f };
        float m_mid1_width{ 1.0f };

        float m_mid2_gain{ 1.0f };
        float m_mid2_center { 3000.0f };
        float m_mid2_width{ 1.0f };

        float m_high_gain{ 1.0f };
        float m_high_cutoff{ 6000.0f };
    };
}

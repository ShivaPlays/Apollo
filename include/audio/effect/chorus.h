//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class chorus : public effect_interface
    {
    public:
        enum class waveform
        {
            sinusoid,
            triangle,

            count
        };

    public:
        void set_waveform(waveform value);
        waveform get_waveform() const;

        void set_phase(int32_t value);
        int32_t get_phase() const;

        void set_rate(float value);
        float get_rate() const;

        void set_depth(float value);
        float get_depth() const;

        void set_feedback(float value);
        float get_feedback() const;

        void set_delay(float value);
        float get_delay() const;

    protected:
        void init() const override;

    private:
        int32_t m_waveform{ 1 };
        int32_t m_phase{ 90 };
        float m_rate{ 1.1f };
        float m_depth{ 0.1f };
        float m_feedback{ 0.25f };
        float m_delay{ 0.016f };
    };
}
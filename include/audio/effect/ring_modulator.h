//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "audio/effect/effect_interface.h"

namespace age::audio::effect
{
    class ring_modulator : public effect_interface
    {
    public:
        enum class waveform
        {
            sinusoid,
            sawtooth,
            square,

            count
        };

    public:
        void set_frequency(float value);
        float get_frequency() const;

        void set_highpass_cutoff(float value);
        float get_highpass_cutoff() const;

        void set_waveform(waveform value);
        waveform get_waveform() const;

    protected:
        void init() override;

    private:
        float m_frequency{ 440.0f };
        float m_highpass_cutoff{ 800.0f };
        int32_t m_waveform{ 0 };
    };
}
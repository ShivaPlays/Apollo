//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class vocal_morpher : public effect_interface
    {
    public:
        enum class waveform
        {
            sinusoid,
            triangle,
            sawtooth,

            count
        };

        enum class phoneme
        {
            a,
            e,
            i,
            o,
            u,
            aa,
            ae,
            ah,
            ao,
            eh,
            er,
            ih,
            iy,
            uh,
            uw,
            b,
            d,
            f,
            g,
            j,
            k,
            l,
            m,
            n,
            p,
            r,
            s,
            t,
            v,
            z,

            count
        };

    public:
        void set_phoneme_a(phoneme value);
        phoneme get_phoneme_a() const;

        void set_phoneme_a_coarse_tuning(int32_t value);
        int32_t get_phoneme_a_coarse_tuning() const;

        void set_phoneme_b(phoneme value);
        phoneme get_phoneme_b() const;

        void set_phoneme_b_coarse_tuning(int32_t value);
        int32_t get_phoneme_b_coarse_tuning() const;

        void set_waveform(waveform value);
        waveform get_waveform() const;

        void set_rate(float value);
        float get_rate() const;

    protected:
        void init() const override;

    private:
        int32_t m_phoneme_a{ 0 };
        int32_t m_phoneme_a_coarse_tuning{ 0 };

        int32_t m_phoneme_b{ 10 };
        int32_t m_phoneme_b_coarse_tuning{ 0 };

        int32_t m_waveform{ 0 };

        float m_rate{ 1.41f };
    };
}
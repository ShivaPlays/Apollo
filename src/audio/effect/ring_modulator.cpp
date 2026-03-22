//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/ring_modulator.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void ring_modulator::set_frequency(float value)
    {
        if (m_frequency != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_RING_MODULATOR_FREQUENCY, value));
            m_frequency = value;
        }
    }

    float ring_modulator::get_frequency() const
    {
        return m_frequency;
    }

    void ring_modulator::set_highpass_cutoff(float value)
    {
        if (m_highpass_cutoff != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_RING_MODULATOR_HIGHPASS_CUTOFF, value));
            m_highpass_cutoff = value;
        }
    }

    float ring_modulator::get_highpass_cutoff() const
    {
        return m_highpass_cutoff;
    }

    void ring_modulator::set_waveform(waveform value)
    {
        constexpr std::array<int32_t, 3> al_waveform{ AL_RING_MODULATOR_SINUSOID, AL_RING_MODULATOR_SAWTOOTH, AL_RING_MODULATOR_SQUARE };
        ALint waveform = AL_RING_MODULATOR_SINUSOID;
        if (value < waveform::count) waveform = al_waveform[static_cast<size_t>(value)];

        if (m_waveform != waveform)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_RING_MODULATOR_WAVEFORM, waveform));
            m_waveform = waveform;
        }
    }

    ring_modulator::waveform ring_modulator::get_waveform() const
    {
        switch (m_waveform)
        {
            case AL_RING_MODULATOR_SINUSOID: return waveform::sinusoid;
            case AL_RING_MODULATOR_SAWTOOTH: return waveform::sawtooth;
            case AL_RING_MODULATOR_SQUARE:   return waveform::square;
            default:                         return waveform::sinusoid;
        }
    }

    void ring_modulator::init() const
    {
        auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_RING_MODULATOR));

        AL_CALL(alEffectf(handle, AL_RING_MODULATOR_FREQUENCY, m_frequency));
        AL_CALL(alEffectf(handle, AL_RING_MODULATOR_HIGHPASS_CUTOFF, m_highpass_cutoff));
        AL_CALL(alEffecti(handle, AL_RING_MODULATOR_WAVEFORM, m_waveform));
    }
}

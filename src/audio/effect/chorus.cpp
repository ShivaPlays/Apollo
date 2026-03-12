//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/chorus.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void chorus::set_waveform(waveform value)
    {
        constexpr std::array<int32_t, 2> al_waveform{ AL_CHORUS_WAVEFORM_SINUSOID,  AL_CHORUS_WAVEFORM_TRIANGLE };

        int32_t waveform = AL_CHORUS_WAVEFORM_SINUSOID;
        if (value < waveform::count) waveform = al_waveform[static_cast<size_t>(value)];

        if (m_waveform != waveform)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_CHORUS_WAVEFORM, waveform));
            m_waveform = waveform;
        }
    }

    chorus::waveform chorus::get_waveform() const
    {
        switch (m_waveform)
        {
            case AL_CHORUS_WAVEFORM_SINUSOID:   return waveform::sinusoid;
            case AL_CHORUS_WAVEFORM_TRIANGLE:   return waveform::triangle;
            default:                            return waveform::sinusoid;
        }
    }

    void chorus::set_phase(int32_t value)
    {
        if (m_phase != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_CHORUS_PHASE, value));
            m_phase = value;
        }
    }

    int32_t chorus::get_phase() const
    {
        return m_phase;
    }

    void chorus::set_rate(float value)
    {
        if (m_rate != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_CHORUS_RATE, value));
            m_rate = value;
        }
    }

    float chorus::get_rate() const
    {
        return m_rate;
    }

    void chorus::set_depth(float value)
    {
        if (m_depth != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_CHORUS_DEPTH, value));
            m_depth = value;
        }
    }

    float chorus::get_depth() const
    {
        return m_depth;
    }

    void chorus::set_feedback(float value)
    {
        if (m_feedback != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_CHORUS_FEEDBACK, value));
            m_feedback = value;
        }
    }

    float chorus::get_feedback() const
    {
        return m_feedback;
    }

    void chorus::set_delay(float value)
    {
        if (m_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_CHORUS_DELAY, value));
            m_delay = value;
        }
    }

    void chorus::init()
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_CHORUS));

        AL_CALL(alEffecti(handle, AL_CHORUS_WAVEFORM, m_waveform));
        AL_CALL(alEffecti(handle, AL_CHORUS_PHASE, m_phase));
        AL_CALL(alEffectf(handle, AL_CHORUS_RATE, m_rate));
        AL_CALL(alEffectf(handle, AL_CHORUS_DEPTH, m_depth));
        AL_CALL(alEffectf(handle, AL_CHORUS_FEEDBACK, m_feedback));
        AL_CALL(alEffectf(handle, AL_CHORUS_DELAY, m_delay));
    }
}
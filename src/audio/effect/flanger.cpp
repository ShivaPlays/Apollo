//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/flanger.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void flanger::set_waveform(waveform value)
    {
        constexpr std::array<int32_t, 2> al_waveform{ AL_FLANGER_WAVEFORM_SINUSOID,  AL_FLANGER_WAVEFORM_TRIANGLE };
        ALint waveform = AL_FLANGER_WAVEFORM_SINUSOID;
        if (value < waveform::count) waveform = al_waveform[static_cast<size_t>(value)];

        if (m_waveform != waveform)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_FLANGER_WAVEFORM, waveform));
            m_waveform = waveform;
        }
    }

    flanger::waveform flanger::get_waveform() const
    {
        switch (m_waveform)
        {
            case AL_FLANGER_WAVEFORM_SINUSOID:  return waveform::sinusoid;
            case AL_FLANGER_WAVEFORM_TRIANGLE:  return waveform::triangle;
            default:                            return waveform::sinusoid;
        }
    }

    void flanger::set_phase(int32_t value)
    {
        if (m_phase != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_FLANGER_PHASE, value));
            m_phase = value;
        }
    }

    int32_t flanger::get_phase() const
    {
        return m_phase;
    }

    void flanger::set_rate(float value)
    {
        if (m_rate != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_FLANGER_RATE, value));
            m_rate = value;
        }
    }

    float flanger::get_rate() const
    {
        return m_rate;
    }

    void flanger::set_depth(float value)
    {
        if (m_depth != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_FLANGER_DEPTH, value));
            m_depth = value;
        }
    }

    float flanger::get_depth() const
    {
        return m_depth;
    }

    void flanger::set_feedback(float value)
    {
        if (m_feedback != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_FLANGER_FEEDBACK, value));
            m_feedback = value;
        }
    }

    float flanger::get_feedback() const
    {
        return m_feedback;
    }

    void flanger::set_delay(float value)
    {
        if (m_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_FLANGER_DELAY, value));
            m_delay = value;
        }
    }

    float flanger::get_delay() const
    {
        return m_delay;
    }

    void flanger::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_FLANGER));

        AL_CALL(alEffecti(handle, AL_FLANGER_WAVEFORM, m_waveform));
        AL_CALL(alEffecti(handle, AL_FLANGER_PHASE, m_phase));
        AL_CALL(alEffectf(handle, AL_FLANGER_RATE, m_rate));
        AL_CALL(alEffectf(handle, AL_FLANGER_DEPTH, m_depth));
        AL_CALL(alEffectf(handle, AL_FLANGER_FEEDBACK, m_feedback));
        AL_CALL(alEffectf(handle, AL_FLANGER_DELAY, m_delay));
    }
}
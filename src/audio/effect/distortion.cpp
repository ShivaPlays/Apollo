//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/distortion.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void distortion::set_edge(float value)
    {
        if (m_edge != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_DISTORTION_EDGE, value));
            m_edge = value;
        }
    }

    float distortion::get_edge() const
    {
        return m_edge;
    }

    void distortion::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_DISTORTION_GAIN, value));
            m_gain = value;
        }
    }

    float distortion::get_gain() const
    {
        return m_gain;
    }

    void distortion::set_lowpass_cutoff(float value)
    {
        if (m_lowpass_cutoff != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_DISTORTION_LOWPASS_CUTOFF, value));
            m_lowpass_cutoff = value;
        }
    }

    float distortion::get_lowpass_cutoff() const
    {
        return m_lowpass_cutoff;
    }

    void distortion::set_eq_center(float value)
    {
        if (m_eq_center != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_DISTORTION_EQCENTER, value));
            m_eq_center = value;
        }
    }

    float distortion::get_eq_center() const
    {
        return m_eq_center;
    }

    void distortion::set_eq_bandwidth(float value)
    {
        if (m_eq_bandwidth != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_DISTORTION_EQBANDWIDTH, value));
            m_eq_bandwidth = value;
        }
    }

    float distortion::get_eq_bandwidth() const
    {
        return m_eq_bandwidth;
    }

    void distortion::init()
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION));

        AL_CALL(alEffectf(handle, AL_DISTORTION_EDGE, m_edge));
        AL_CALL(alEffectf(handle, AL_DISTORTION_GAIN, m_gain));
        AL_CALL(alEffectf(handle, AL_DISTORTION_LOWPASS_CUTOFF, m_lowpass_cutoff));
        AL_CALL(alEffectf(handle, AL_DISTORTION_EQCENTER, m_eq_center));
        AL_CALL(alEffectf(handle, AL_DISTORTION_EQBANDWIDTH, m_eq_bandwidth));
    }
}

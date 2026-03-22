//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/equalizer.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void equalizer::set_low_gain(float value)
    {
        if (m_low_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_LOW_GAIN, value));
            m_low_gain = value;
        }
    }

    float equalizer::get_low_gain() const
    {
        return m_low_gain;
    }

    void equalizer::set_low_cutoff(float value)
    {
        if (m_low_cutoff != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_LOW_CUTOFF, value));
            m_low_cutoff = value;
        }
    }

    float equalizer::get_low_cutoff() const
    {
        return m_low_cutoff;
    }

    void equalizer::set_mid1_gain(float value)
    {
        if (m_mid1_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID1_GAIN, value));
            m_mid1_gain = value;
        }
    }

    float equalizer::get_mid1_gain() const
    {
        return m_mid1_gain;
    }

    void equalizer::set_mid1_center(float value)
    {
        if (m_mid1_center != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID1_CENTER, value));
            m_mid1_center = value;
        }
    }

    float equalizer::get_mid1_center() const
    {
        return m_mid1_center;
    }

    void equalizer::set_mid1_width(float value)
    {
        if (m_mid1_width != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID1_WIDTH, value));
            m_mid1_width = value;
        }
    }

    float equalizer::get_mid1_width() const
    {
        return m_mid1_width;
    }

    void equalizer::set_mid2_gain(float value)
    {
        if (m_mid2_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID2_GAIN, value));
            m_mid2_gain = value;
        }
    }

    float equalizer::get_mid2_gain() const
    {
        return m_mid2_gain;
    }

    void equalizer::set_mid2_center(float value)
    {
        if (m_mid2_center != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID2_CENTER, value));
            m_mid2_center = value;
        }
    }

    float equalizer::get_mid2_center() const
    {
        return m_mid2_center;
    }

    void equalizer::set_mid2_width(float value)
    {
        if (m_mid2_width != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_MID2_WIDTH, value));
            m_mid2_width = value;
        }
    }

    float equalizer::get_mid2_width() const
    {
        return m_mid2_width;
    }

    void equalizer::set_high_gain(float value)
    {
        if (m_high_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_HIGH_GAIN, value));
            m_high_gain = value;
        }
    }

    float equalizer::get_high_gain() const
    {
        return m_high_gain;
    }

    void equalizer::set_high_cutoff(float value)
    {
        if (m_high_cutoff != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_EQUALIZER_HIGH_CUTOFF, value));
            m_high_cutoff = value;
        }
    }

    float equalizer::get_high_cutoff() const
    {
        return m_high_cutoff;
    }

    void equalizer::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER));

        AL_CALL(alEffectf(handle, AL_EQUALIZER_LOW_GAIN, m_low_gain));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_LOW_CUTOFF, m_low_cutoff));

        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID1_GAIN, m_mid1_gain));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID1_CENTER, m_mid1_center));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID1_WIDTH, m_mid1_width));

        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID2_GAIN, m_mid2_gain));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID2_CENTER, m_mid2_center));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_MID2_WIDTH, m_mid2_width));

        AL_CALL(alEffectf(handle, AL_EQUALIZER_HIGH_GAIN, m_high_gain));
        AL_CALL(alEffectf(handle, AL_EQUALIZER_HIGH_CUTOFF, m_high_cutoff));
    }
}

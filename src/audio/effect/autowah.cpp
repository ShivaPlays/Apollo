//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/autowah.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void autowah::set_attack_time(float value)
    {
        if (m_attack_time != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_AUTOWAH_ATTACK_TIME, value));
            m_attack_time = value;
        }
    }

    float autowah::get_attack_time() const
    {
        return m_attack_time;
    }

    void autowah::set_release_time(float value)
    {
        if (m_release_time != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_AUTOWAH_RELEASE_TIME, value));
            m_release_time = value;
        }
    }

    float autowah::get_release_time() const
    {
        return m_release_time;
    }

    void autowah::set_resonance(float value)
    {
        if (m_resonance != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_AUTOWAH_RESONANCE, value));
            m_resonance = value;
        }
    }

    float autowah::get_resonance() const
    {
        return m_resonance;
    }

    void autowah::set_peak_gain(float value)
    {
        if (m_peak_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_AUTOWAH_PEAK_GAIN, value));
            m_peak_gain = value;
        }
    }

    float autowah::get_peak_gain() const
    {
        return m_peak_gain;
    }

    void autowah::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_AUTOWAH));

        AL_CALL(alEffectf(handle, AL_AUTOWAH_ATTACK_TIME, m_attack_time));
        AL_CALL(alEffectf(handle, AL_AUTOWAH_RELEASE_TIME, m_release_time));
        AL_CALL(alEffectf(handle, AL_AUTOWAH_RESONANCE, m_resonance));
        AL_CALL(alEffectf(handle, AL_AUTOWAH_PEAK_GAIN, m_peak_gain));
    }
}

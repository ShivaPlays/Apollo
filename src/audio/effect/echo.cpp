//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/echo.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void echo::set_delay(float value)
    {
        if (m_delay != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_ECHO_DELAY, value));
            m_delay = value;
        }
    }

    float echo::get_delay() const
    {
        return m_delay;
    }

    void echo::set_lr_delay(float value)
    {
        if (m_lr_delay != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_ECHO_LRDELAY, value));
            m_lr_delay = value;
        }
    }

    float echo::get_lr_delay() const
    {
        return m_lr_delay;
    }

    void echo::set_feedback(float value)
    {
        if (m_feedback != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_ECHO_FEEDBACK, value));
            m_feedback = value;
        }
    }

    float echo::get_feedback() const
    {
        return m_feedback;
    }

    void echo::set_damping(float value)
    {
        if (m_damping != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_ECHO_DAMPING, value));
            m_damping = value;
        }
    }

    float echo::get_damping() const
    {
        return m_damping;
    }

    void echo::set_spread(float value)
    {
        if (m_spread != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_ECHO_SPREAD, value));
            m_spread = value;
        }
    }

    float echo::get_spread() const
    {
        return m_spread;
    }

    void echo::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_ECHO));

        AL_CALL(alEffectf(handle, AL_ECHO_DELAY, m_delay));
        AL_CALL(alEffectf(handle, AL_ECHO_LRDELAY, m_lr_delay));
        AL_CALL(alEffectf(handle, AL_ECHO_FEEDBACK, m_feedback));
        AL_CALL(alEffectf(handle, AL_ECHO_DAMPING, m_damping));
        AL_CALL(alEffectf(handle, AL_ECHO_SPREAD, m_spread));
    }
}

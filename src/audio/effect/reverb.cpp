//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/reverb.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void reverb::set_air_absorption_gain_hf(float value)
    {
        if (m_air_absorption_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_AIR_ABSORPTION_GAINHF, value));
            m_air_absorption_gain_hf = value;
        }
    }

    float reverb::get_air_absorption_gain_hf() const
    {
        return m_air_absorption_gain_hf;
    }

    void reverb::set_density(float value)
    {
        if (m_density != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_DENSITY, value));
            m_density = value;
        }
    }

    float reverb::get_density() const
    {
        return m_density;
    }

    void reverb::set_diffusion(float value)
    {
        if (m_diffusion != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_DIFFUSION, value));
            m_diffusion = value;
        }
    }

    float reverb::get_diffusion() const
    {
        return m_diffusion;
    }

    void reverb::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_GAIN, value));
            m_gain = value;
        }
    }

    float reverb::get_gain() const
    {
        return m_gain;
    }

    void reverb::set_gain_hf(float value)
    {
        if (m_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_GAINHF, value));
            m_gain_hf = value;
        }
    }

    float reverb::get_gain_hf() const
    {
        return m_gain_hf;
    }

    void reverb::set_decay_time(float value)
    {
        if (m_decay_time != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_DECAY_TIME, value));
            m_decay_time = value;
        }
    }

    float reverb::get_decay_time() const
    {
        return m_decay_time;
    }

    void reverb::set_decay_hf_ratio(float value)
    {
        if (m_decay_hf_ratio != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_DECAY_HFRATIO, value));
            m_decay_hf_ratio = value;
        }
    }

    float reverb::get_decay_hf_ratio() const
    {
        return m_decay_hf_ratio;
    }

    void reverb::set_reflections_gain(float value)
    {
        if (m_reflections_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_REFLECTIONS_GAIN, value));
            m_reflections_gain = value;
        }
    }

    float reverb::get_reflections_gain() const
    {
        return m_reflections_gain;
    }

    void reverb::set_reflections_delay(float value)
    {
        if (m_reflections_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_REFLECTIONS_DELAY, value));
            m_reflections_delay = value;
        }
    }

    float reverb::get_reflections_delay() const
    {
        return m_reflections_delay;
    }

    void reverb::set_late_reverb_gain(float value)
    {
        if (m_late_reverb_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_LATE_REVERB_GAIN, value));
            m_late_reverb_gain = value;
        }
    }

    float reverb::get_late_reverb_gain() const
    {
        return m_late_reverb_gain;
    }

    void reverb::set_late_reverb_delay(float value)
    {
        if (m_late_reverb_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_LATE_REVERB_DELAY, value));
            m_late_reverb_delay = value;
        }
    }

    float reverb::get_late_reverb_delay() const
    {
        return m_late_reverb_delay;
    }

    void reverb::set_room_rolloff_factor(float value)
    {
        if (m_room_rolloff_factor != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_REVERB_ROOM_ROLLOFF_FACTOR, value));
            m_room_rolloff_factor = value;
        }
    }

    float reverb::get_room_rolloff_factor() const
    {
        return m_room_rolloff_factor;
    }

    void reverb::set_decay_hfl_limit(bool value)
    {
        if (m_decay_hfl_limit != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_REVERB_DECAY_HFLIMIT, value ? 1 : 0));
            m_decay_hfl_limit = value;
        }
    }

    bool reverb::get_decay_hfl_limit() const
    {
        return m_decay_hfl_limit;
    }

    void reverb::init()
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_REVERB));

        AL_CALL(alEffectf(handle, AL_REVERB_AIR_ABSORPTION_GAINHF, m_air_absorption_gain_hf));

        AL_CALL(alEffectf(handle, AL_REVERB_DENSITY, m_density));
        AL_CALL(alEffectf(handle, AL_REVERB_DIFFUSION, m_diffusion));

        AL_CALL(alEffectf(handle, AL_REVERB_GAIN, m_gain));
        AL_CALL(alEffectf(handle, AL_REVERB_GAINHF, m_gain_hf));

        AL_CALL(alEffectf(handle, AL_REVERB_DECAY_TIME, m_decay_time));
        AL_CALL(alEffectf(handle, AL_REVERB_DECAY_HFRATIO, m_decay_hf_ratio));

        AL_CALL(alEffectf(handle, AL_REVERB_REFLECTIONS_GAIN, m_reflections_gain));
        AL_CALL(alEffectf(handle, AL_REVERB_REFLECTIONS_DELAY, m_reflections_delay));

        AL_CALL(alEffectf(handle, AL_REVERB_LATE_REVERB_GAIN, m_late_reverb_gain));
        AL_CALL(alEffectf(handle, AL_REVERB_LATE_REVERB_DELAY, m_late_reverb_delay));

        AL_CALL(alEffectf(handle, AL_REVERB_ROOM_ROLLOFF_FACTOR, m_room_rolloff_factor));

        AL_CALL(alEffecti(handle, AL_REVERB_DECAY_HFLIMIT, m_decay_hfl_limit ? 1 : 0));
    }
}

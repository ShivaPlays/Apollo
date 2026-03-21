//
// Created by skaldi on 21.03.26.
//

#include "audio/effect/eax_reverb.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

#include <array>

namespace age::audio::effect
{
    void eax_reverb::set_density(float value)
    {
        if (m_density != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_DENSITY, value));

            m_density = value;
        }
    }

    float eax_reverb::get_density() const
    {
        return m_density;
    }

    void eax_reverb::set_diffusion(float value)
    {
        if (m_diffusion != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_DIFFUSION, value));

            m_diffusion = value;
        }
    }

    float eax_reverb::get_diffusion() const
    {
        return m_diffusion;
    }

    void eax_reverb::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_GAIN, value));

            m_gain = value;
        }
    }

    float eax_reverb::get_gain() const
    {
        return m_gain;
    }

    void eax_reverb::set_gain_hf(float value)
    {
        if (m_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_GAINHF, value));

            m_gain_hf = value;
        }
    }

    float eax_reverb::get_gain_hf() const
    {
        return m_gain_hf;
    }

    void eax_reverb::set_gain_lf(float value)
    {
        if (m_gain_lf != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_GAINLF, value));

            m_gain_lf = value;
        }
    }

    float eax_reverb::get_gain_lf() const
    {
        return m_gain_lf;
    }

    void eax_reverb::set_decay_time(float value)
    {
        if (m_decay_time != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_DECAY_TIME, value));

            m_decay_time = value;
        }
    }

    float eax_reverb::get_decay_time() const
    {
        return m_decay_time;
    }

    void eax_reverb::set_decay_hf_ratio(float value)
    {
        if (m_decay_hf_ratio != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_DECAY_HFRATIO, value));

            m_decay_hf_ratio = value;
        }
    }

    float eax_reverb::get_decay_hf_ratio() const
    {
        return m_decay_hf_ratio;
    }

    void eax_reverb::set_decay_lf_ratio(float value)
    {
        if (m_decay_lf_ratio != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_DECAY_LFRATIO, value));

            m_decay_lf_ratio = value;
        }
    }

    float eax_reverb::get_decay_lf_ratio() const
    {
        return m_decay_lf_ratio;
    }

    void eax_reverb::set_reflections_gain(float value)
    {
        if (m_reflections_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_REFLECTIONS_GAIN, value));

            m_reflections_gain = value;
        }
    }

    float eax_reverb::get_reflections_gain() const
    {
        return m_reflections_gain;
    }

    void eax_reverb::set_reflections_delay(float value)
    {
        if (m_reflections_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_REFLECTIONS_DELAY, value));

            m_reflections_delay = value;
        }
    }

    float eax_reverb::get_reflections_delay() const
    {
        return m_reflections_delay;
    }

    void eax_reverb::set_reflections_pan(const glm::vec3& value)
    {
        if (m_reflections_pan != value)
        {
            if (ensure_handle()) AL_CALL(alEffectfv(get_handle(), AL_EAXREVERB_REFLECTIONS_PAN, std::array<ALfloat, 3>{ value.x, value.y, value.z }.data()));

            m_reflections_pan = value;
        }
    }

    const glm::vec3& eax_reverb::get_reflections_pan() const
    {
        return m_reflections_pan;
    }

    void eax_reverb::set_late_reverb_gain(float value)
    {
        if (m_late_reverb_gain != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_LATE_REVERB_GAIN, value));

            m_late_reverb_gain = value;
        }
    }

    float eax_reverb::get_late_reverb_gain() const
    {
        return m_late_reverb_gain;
    }

    void eax_reverb::set_late_reverb_delay(float value)
    {
        if (m_late_reverb_delay != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_LATE_REVERB_DELAY, value));

            m_late_reverb_delay = value;
        }
    }

    float eax_reverb::get_late_reverb_delay() const
    {
        return m_late_reverb_delay;
    }

    void eax_reverb::set_late_reverb_pan(const glm::vec3& value)
    {
        if (m_late_reverb_pan != value)
        {
            if (ensure_handle()) AL_CALL(alEffectfv(get_handle(), AL_EAXREVERB_LATE_REVERB_PAN, std::array<ALfloat, 3>{ value.x, value.y, value.z }.data()));

            m_late_reverb_pan = value;
        }
    }

    const glm::vec3& eax_reverb::get_late_reverb_pan() const
    {
        return m_late_reverb_pan;
    }

    void eax_reverb::set_echo_time(float value)
    {
        if (m_echo_time != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_ECHO_TIME, value));

            m_echo_time = value;
        }
    }

    float eax_reverb::get_echo_time() const
    {
        return m_echo_time;
    }

    void eax_reverb::set_echo_depth(float value)
    {
        if (m_echo_depth != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_ECHO_DEPTH, value));

            m_echo_depth = value;
        }
    }

    float eax_reverb::get_echo_depth() const
    {
        return m_echo_depth;
    }

    void eax_reverb::set_modulation_time(float value)
    {
        if (m_modulation_time != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_MODULATION_TIME, value));

            m_modulation_time = value;
        }
    }

    float eax_reverb::get_modulation_time() const
    {
        return m_modulation_time;
    }

    void eax_reverb::set_modulation_depth(float value)
    {
        if (m_modulation_depth != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_MODULATION_DEPTH, value));

            m_modulation_depth = value;
        }
    }

    float eax_reverb::get_modulation_depth() const
    {
        return m_modulation_depth;
    }

    void eax_reverb::set_air_absorption_gain_hf(float value)
    {
        if (m_air_absorption_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_AIR_ABSORPTION_GAINHF, value));

            m_air_absorption_gain_hf = value;
        }
    }

    float eax_reverb::get_air_absorption_gain_hf() const
    {
        return m_air_absorption_gain_hf;
    }

    void eax_reverb::set_hf_reference(float value)
    {
        if (m_hf_reference != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_HFREFERENCE, value));

            m_hf_reference = value;
        }
    }

    float eax_reverb::get_hf_reference() const
    {
        return m_hf_reference;
    }

    void eax_reverb::set_lf_reference(float value)
    {
        if (m_lf_reference != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_LFREFERENCE, value));

            m_lf_reference = value;
        }
    }

    float eax_reverb::get_lf_reference() const
    {
        return m_lf_reference;
    }

    void eax_reverb::set_room_rolloff_factor(float value)
    {
        if (m_room_rolloff_factor != value)
        {
            if (ensure_handle()) AL_CALL(alEffectf(get_handle(), AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, value));

            m_room_rolloff_factor = value;
        }
    }

    float eax_reverb::get_room_rolloff_factor() const
    {
        return m_room_rolloff_factor;
    }

    void eax_reverb::set_decay_hf_limit(bool value)
    {
        if (m_decay_hf_limit != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_EAXREVERB_DECAY_HFLIMIT, value ? AL_TRUE : AL_FALSE));

            m_decay_hf_limit = value;
        }
    }

    bool eax_reverb::get_decay_hf_limit() const
    {
        return m_decay_hf_limit;
    }

    void eax_reverb::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB));

        AL_CALL(alEffectf(handle, AL_EAXREVERB_DENSITY, m_density));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_DIFFUSION, m_diffusion));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_GAIN, m_gain));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_GAINHF, m_gain_hf));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_GAINLF, m_gain_lf));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_DECAY_TIME, m_decay_time));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_DECAY_HFRATIO, m_decay_hf_ratio));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_DECAY_LFRATIO, m_decay_lf_ratio));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_REFLECTIONS_GAIN, m_reflections_gain));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_REFLECTIONS_DELAY, m_reflections_delay));
        AL_CALL(alEffectfv(handle, AL_EAXREVERB_REFLECTIONS_PAN, std::array<ALfloat, 3>{ m_reflections_pan.x, m_reflections_pan.y, m_reflections_pan.z }.data()));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_LATE_REVERB_GAIN, m_late_reverb_gain));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_LATE_REVERB_DELAY, m_late_reverb_delay));
        AL_CALL(alEffectfv(handle, AL_EAXREVERB_LATE_REVERB_PAN, std::array<ALfloat, 3>{ m_late_reverb_pan.x, m_late_reverb_pan.y, m_late_reverb_pan.z }.data()));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_ECHO_TIME, m_echo_time));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_ECHO_DEPTH, m_echo_depth));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_MODULATION_TIME, m_modulation_time));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_MODULATION_DEPTH, m_modulation_depth));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, m_air_absorption_gain_hf));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_HFREFERENCE, m_hf_reference));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_LFREFERENCE, m_lf_reference));
        AL_CALL(alEffectf(handle, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, m_room_rolloff_factor));
        AL_CALL(alEffecti(handle, AL_EAXREVERB_DECAY_HFLIMIT, m_decay_hf_limit ? AL_TRUE : AL_FALSE));
    }
}

//
// Created by skaldi on 21.03.26.
//

#include "audio/effect/dedicated_dialog.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void dedicated_dialog::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_DEDICATED_GAIN, 1.0f));

            m_gain = value;
        }
    }

    float dedicated_dialog::get_gain() const
    {
        return m_gain;
    }

    void dedicated_dialog::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_DEDICATED_DIALOGUE));

        AL_CALL(alEffectf(handle, AL_DEDICATED_GAIN, 1.0f));
    }
}

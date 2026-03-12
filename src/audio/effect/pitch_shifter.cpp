//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/pitch_shifter.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void pitch_shifter::set_coarse_tune(int32_t value)
    {
        if (m_coarse_tune != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_PITCH_SHIFTER_COARSE_TUNE, value));
            m_coarse_tune = value;
        }
    }

    int32_t pitch_shifter::get_coarse_tune() const
    {
        return m_coarse_tune;
    }

    void pitch_shifter::set_fine_tune(int32_t value)
    {
        if (m_fine_tune != value)
        {
            if (ensure_handle()) AL_CALL(alEffecti(get_handle(), AL_PITCH_SHIFTER_FINE_TUNE, value));
            m_fine_tune = value;
        }
    }

    int32_t pitch_shifter::get_fine_tune() const
    {
        return m_fine_tune;
    }

    void pitch_shifter::init()
    {
        auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_PITCH_SHIFTER));

        AL_CALL(alEffecti(handle, AL_PITCH_SHIFTER_COARSE_TUNE, m_coarse_tune));
        AL_CALL(alEffecti(handle, AL_PITCH_SHIFTER_FINE_TUNE, m_fine_tune));
    }
}

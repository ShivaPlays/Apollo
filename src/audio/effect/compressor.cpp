//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/compressor.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    void compressor::set_on_off(bool value)
    {
        if (m_on_off != value)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_COMPRESSOR_ONOFF, m_on_off ? 1 : 0));
            m_on_off = value;
        }
    }

    bool compressor::get_on_off() const
    {
        return m_on_off;
    }

    void compressor::init() const
    {
        const auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_COMPRESSOR));
        AL_CALL(alEffecti(handle, AL_COMPRESSOR_ONOFF, m_on_off ? 1 : 0));
    }
}
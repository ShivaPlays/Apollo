//
// Created by skaldi on 08.03.26.
//

#include "audio/filter/low_pass.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::filter
{
    void low_pass::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_LOWPASS_GAIN, value));
            m_gain = value;
        }
    }

    float low_pass::get_gain() const
    {
        return m_gain;
    }

    void low_pass::set_gain_hf(float value)
    {
        if (m_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_LOWPASS_GAINHF, value));
            m_gain_hf = value;
        }
    }

    float low_pass::get_gain_hf() const
    {
        return m_gain_hf;
    }

    void low_pass::init() const
    {
        AL_CALL(alFilteri(get_handle(), AL_FILTER_TYPE, AL_FILTER_LOWPASS));
        AL_CALL(alFilterf(get_handle(), AL_LOWPASS_GAIN, m_gain));
        AL_CALL(alFilterf(get_handle(), AL_LOWPASS_GAINHF, m_gain_hf));
    }
}

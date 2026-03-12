//
// Created by skaldi on 08.03.26.
//

#include "audio/filter/high_pass.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::filter
{
    void high_pass::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_HIGHPASS_GAIN, value));

            m_gain = value;
        }
    }

    float high_pass::get_gain() const
    {
        return m_gain;
    }

    void high_pass::set_gain_lf(float value)
    {
        if (m_gain_lf != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_HIGHPASS_GAINLF, value));

            m_gain_lf = value;
        }
    }

    float high_pass::get_gain_lf() const
    {
        return m_gain_lf;
    }

    void high_pass::init()
    {
        AL_CALL(alFilteri(get_handle(), AL_FILTER_TYPE, AL_FILTER_HIGHPASS));
        AL_CALL(alFilterf(get_handle(), AL_HIGHPASS_GAIN, m_gain));
        AL_CALL(alFilterf(get_handle(), AL_HIGHPASS_GAINLF, m_gain_lf));
    }
}

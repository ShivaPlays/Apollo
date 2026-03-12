//
// Created by skaldi on 08.03.26.
//

#include "audio/filter/band_pass.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::filter
{
    void band_pass::set_gain(float value)
    {
        if (m_gain != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAIN, value));
            m_gain = value;
        }
    }

    float band_pass::get_gain() const
    {
        return m_gain;
    }

    void band_pass::set_gain_lf(float value)
    {
        if (m_gain_lf != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAINLF, value));
            m_gain_lf = value;
        }
    }

    float band_pass::get_gain_lf() const
    {
        return m_gain_lf;
    }

    void band_pass::set_gain_hf(float value)
    {
        if (m_gain_hf != value)
        {
            if (ensure_handle()) AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAINHF, value));
            m_gain_hf = value;
        }
    }

    float band_pass::get_gain_hf() const
    {
        return m_gain_hf;
    }

    void band_pass::init()
    {
        AL_CALL(alFilteri(get_handle(), AL_FILTER_TYPE, AL_FILTER_BANDPASS));
        AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAIN, m_gain));
        AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAINLF, m_gain_lf));
        AL_CALL(alFilterf(get_handle(), AL_BANDPASS_GAINHF, m_gain_hf));
    }
}

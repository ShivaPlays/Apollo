//
// Created by skaldi on 08.03.26.
//

#include "audio/filter/band_pass.h"

#include <AL/alext.h>

namespace age::audio::filter
{
    uint32_t band_pass::get_type() const
    {
        return AL_FILTER_BANDPASS;
    }
}

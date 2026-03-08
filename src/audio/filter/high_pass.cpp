//
// Created by skaldi on 08.03.26.
//

#include "audio/filter/high_pass.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "utility/al_check.h"

namespace age::audio::filter
{
    int32_t high_pass::get_type() const
    {
        return AL_FILTER_BANDPASS;
    }
}

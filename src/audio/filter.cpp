//
// Created by skaldi on 22.03.26.
//

#include "audio/filter.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio
{
    uint32_t filter::gen_handle()
    {
        ALuint handle{};

        return handle;
    }
}

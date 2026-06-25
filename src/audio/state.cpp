//
// Created by skaldi on 24.06.26.
//

#include "audio/state.h"

#include <AL/al.h>

namespace age::audio
{
    state i_to_state(uint32_t value)
    {
        switch (value)
        {
        case AL_PLAYING: return state::playing;
        case AL_PAUSED: return state::paused;
        default: return state::stopped;
        }
    }
}

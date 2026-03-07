//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/effect_interface.h"

#include "audio/effect/slot.h"

namespace age::audio::effect
{
    void effect_interface::notify_death()
    {
        m_tracker.notify_and_clear([this](slot* s) {
            s->on_effect_destroyed(this);
        });
    }
}

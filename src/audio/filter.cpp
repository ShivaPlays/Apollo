//
// Created by skaldi on 06.03.26.
//

#include "audio/filter.h"

#include "audio/effect/slot.h"

namespace age::audio
{
    void filter::notify_death()
    {
        m_tracker.notify_and_clear([this](effect::slot* s)
        {
            s->on_filter_destroyed(this);
        });
    }
}

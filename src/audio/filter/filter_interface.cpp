//
// Created by skaldi on 06.03.26.
//

#include "audio/filter/filter_interface.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/effect/slot.h"
#include "utility/al_check.h"


namespace age::audio
{
    uint32_t filter_interface::gen_handle()
    {
        ALuint handle{};
        AL_CALL(alGenFilters(1, &handle));

        return handle;
    }

    void filter_interface::delete_handle(uint32_t value)
    {
        alDeleteFilters(1, &value);
    }

    bool filter_interface::ensure_handle()
    {
        if (m_handle == 0)
        {
            m_handle = gen_handle();
            if (m_handle) AL_CALL(alFilteri(m_handle, AL_FILTER_TYPE, get_type()));
        }

        return m_handle != 0;
    }

    void filter_interface::notify_death()
    {
        m_tracker.notify_and_clear([this](effect::slot* s)
        {
            s->on_filter_destroyed(this);
        });
    }
}

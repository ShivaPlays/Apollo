//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/effect_interface.h"

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/effect/slot.h"
#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    uint32_t effect_interface::gen_handle()
    {
        ALuint handle{};
        AL_CALL(alGenEffects(1, &handle));

        return handle;
    }

    void effect_interface::delete_handle(uint32_t value)
    {
        AL_CALL(alDeleteEffects(1, &value));
    }

    bool effect_interface::realize() const
    {
        if (m_handle == 0)
        {
            m_handle = gen_handle();
            if (m_handle) init();
        }

        return m_handle != 0;
    }

    void effect_interface::notify_death() const
    {
        m_tracker.notify_and_clear([this](slot* s) {
            s->on_effect_destroyed(this);
        });
    }
}

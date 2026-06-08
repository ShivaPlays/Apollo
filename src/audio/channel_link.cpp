//
// Created by skaldi on 08.06.26.
//

#include "audio/channel_link.h"

#include "audio/sound_interface.h"

namespace age::audio
{
    void channel_link::trigger_channel_loss()
    {
        m_owner->on_channel_lost();
    }
}

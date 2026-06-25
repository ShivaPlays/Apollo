//
// Created by skaldi on 08.06.26.
//

#include "audio/channel_link.h"

#include "audio/sound_interface.h"

namespace age::audio
{
    void channel_link::trigger_channel_loss(channel& channel)
    {
        m_owner->on_channel_lost(channel);
    }

    void channel_link::tirgger_queued_buffers_processed(size_t num_buffers)
    {
        m_owner->on_queued_buffers_processed(num_buffers);
    }
}

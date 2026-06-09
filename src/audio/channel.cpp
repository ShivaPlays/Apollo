//
// Created by skaldi on 13.03.26.
//

#include "audio/channel.h"

#include "audio/device.h"
#include "audio/channel_link.h"

namespace age::audio
{
    channel::channel(channel&& other) noexcept
        : m_source{ std::move(other.m_source) }
        //, m_owner{ std::exchange(other.m_owner, nullptr) }
        , m_busy{ other.m_busy.exchange(false, std::memory_order_relaxed) }
        , m_priority{ std::exchange(other.m_priority, 0) }
        , m_is_reserved{ std::exchange(other.m_is_reserved, false) }
    {
        // 1. Lock BOTH mutexes on the source channel before touching its pointers
        std::scoped_lock lock{ other.m_state_mutex, other.m_owner_mutex };

        m_owner = std::exchange(other.m_owner, nullptr);
        if (m_owner) m_owner->update_moved_channel(this);
    }

    void channel::set_owner(channel_link* owner)
    {
        std::scoped_lock lock{ m_owner_mutex };

        if (m_owner && m_owner != owner) m_owner->notify_channel_lost();
        m_owner = owner;
    }

    void channel::stop_and_release()
    {
        std::scoped_lock lock{ m_state_mutex, m_owner_mutex };

        if (m_owner) m_owner->notify_channel_lost();
        m_owner = nullptr;

        stop();
    }

    void channel::apply_auxiliary_bus()
    {
        //bus 0 is reserved for no bus
        if (m_auxiliary_bus == 0)
        {
            m_source.reset_effect_slots();
            return;
        }

        const auto& aux_group = device::get().get_auxiliary_send_group(m_auxiliary_bus);
        if (!aux_group.get_enabled())
        {
            m_source.reset_effect_slots();
            return;
        }

        m_source.update_effect_slots(aux_group);
    }
}

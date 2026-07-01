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
        , m_priority{ std::exchange(other.m_priority, 0) }
        , m_is_reserved{ std::exchange(other.m_is_reserved, false) }
    {
        // 1. Lock BOTH mutexes on the source channel before touching its pointers
        std::scoped_lock lock{ other.m_state_mutex, other.m_owner };

        m_owner.set(other.m_owner.get());
        other.m_owner.set(nullptr);

        if (m_owner.get()) m_owner.get()->update_moved_channel(this);
    }

    void channel::set_owner(channel_link* owner)
    {
        std::scoped_lock lock{ m_owner };

        if (m_owner.get() && m_owner.get() != owner) m_owner.get()->notify_channel_lost();
        m_owner.set(owner);
    }

    void channel::stop_and_release()
    {
        std::scoped_lock lock{ m_state_mutex, m_owner };

        if (m_owner.get())
        {
            m_owner.get()->notify_channel_lost();
        }

        m_owner.set(nullptr);

        stop();
    }

    void channel::queued_buffers_processed(size_t num_buffers)
    {
        std::scoped_lock lock{ m_owner };

        if (m_owner.get()) m_owner.get()->tirgger_queued_buffers_processed(num_buffers);
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

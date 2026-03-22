//
// Created by skaldi on 13.03.26.
//

#include "audio/channel.h"

#include "audio/device.h"

namespace age::audio
{
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

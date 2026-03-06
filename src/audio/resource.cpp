#include "audio/resource.h"
#include "audio/device.h"

namespace age::audio
{
    resource::resource()
    {
        ++m_num_resources;

        if (m_num_resources == 1)
        {
            if (!device::get().is_initialised())
                device::init();
        }
    }

    resource::~resource()
    {
        --m_num_resources;

        if (!m_num_resources)
        {
            if (device::get().is_initialised())
                device::destroy();
        }
    }
}

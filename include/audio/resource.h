#pragma once

#include <atomic>
#include <cstddef>

namespace age::audio
{
    class resource
    {
    public:
        resource();
        virtual ~resource();
    public:

    protected:

    private:
        inline static std::atomic<size_t> m_num_resources{0};
    };
}
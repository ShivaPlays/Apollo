//
// Created by skaldi on 06.02.26.
//

#pragma once

#include "render_target.h"
#include "texture.h"
#include "../utility/utility.h"

namespace age
{
    class frame_buffer
        : public render_target
    {
    public:
        frame_buffer() = default;

    public:
        bool bind();
        void clear();

        void invalidate();

        static void reset_cache();
    protected:

    private:
        static void delete_handle(uint32_t handle);

        texture m_color_attachment;

        unique_handle <uint32_t, delete_handle> m_handle;
    };
}
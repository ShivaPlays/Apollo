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
    {
    public:
        frame_buffer() = default;

    public:
        void draw(render_target& target, texture& tex, const drawable& drawable_object, const render_states& states);
        void draw(render_target& target, texture& tex, const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states);
        void draw(render_target& target, texture& tex, const vertex_2d vertices[], size_t num_vertices, primitive_type type, const render_states& states);

        void invalidate();

        static void reset_cache();
    protected:

    private:
        bool realize();

        bool bind();
        void reset();

        uint32_t get_handle() const { return m_handle;}

        static void delete_handle(uint32_t handle);
        unique_handle <uint32_t, delete_handle> m_handle;
    };
}
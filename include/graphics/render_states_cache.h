//
// Created by skaldi on 09.02.26.
//

#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>

#include "graphics/rect.h"

namespace age
{
    struct render_state_cache
    {
        uint32_t program = 0;
        uint32_t uniform_buffer_object = 0;
        uint32_t framebuffer = 0;
        uint32_t vertex_array = 0;

        // Match your VBO targets exactly
        std::array<uint32_t, 2> vertex_buffer_objects{0, 0};

        // Add texture units if you want to be thorough
        // std::array<uint32_t, 16> texture_units{0};
        uint32_t texture = 0;

        // The current Projection/View matrix in the UBO
        glm::mat4 projection_matrix{ 1.0f };

        int_rect view{{0,0},{0,0}};

        void invalidate()
        {
            program = 0;
            uniform_buffer_object = 0;
            framebuffer = 0;
            vertex_array = 0;
            vertex_buffer_objects.fill(0);
            //texture_units.fill(0);
            texture = 0;

            projection_matrix = glm::mat4(1.0f);
            view = int_rect{{0, 0}, {0, 0}};
        }
    };

    inline thread_local render_state_cache g_render_state;
}
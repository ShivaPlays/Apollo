//
// Created by skaldi on 02.07.26.
//

#include "graphics/texture_interface.h"

#include <glm/gtc/matrix_transform.hpp>

#include "utility/gl_loader.h"

#include "graphics/render_states_cache.h"
#include "utility/gl_check.h"
#include "engine.h"

namespace age
{
    texture_interface::texture_interface(const texture_interface& other)
        : m_size( other.m_size )
    {}

    texture_interface& texture_interface::operator=(const texture_interface& other)
    {
        if (&other == this) return *this;

        texture_interface temp{ other };
        *this = std::move(temp);

        return *this;
    }

    bool texture_interface::bind() const
    {
        auto handle = get_handle();

        if (handle != g_render_state.texture)
        {
            GL_CALL(glBindTexture(m_type, handle));
            g_render_state.texture = handle;
        }

        return handle != 0;
    }

    bool texture_interface::bind(const texture_interface* tex)
    {
        if (tex)
        {
            return tex->bind();
        }

        if (g_render_state.texture != 0)
        {
            GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
            g_render_state.texture = 0;

            return true;
        }

        return g_render_state.texture != 0;
    }

    void texture_interface::invalidate()
    {
        m_handle.reset(0);
        m_size = glm::uvec3{ 1, 1, 1 };
    }

    void texture_interface::reset_cache()
    {
        g_render_state.texture = 0;
    }

    uint32_t texture_interface::gen_handle()
    {
        GLuint handle;
        GL_CALL(glGenTextures(1, &handle));

        return handle;
    }

    void texture_interface::delete_handle(uint32_t handle)
    {
        if (!engine::is_device_reset()) GL_CALL(glDeleteTextures(1, &handle));
        if (handle == g_render_state.texture) g_render_state.texture = 0;
    }
}

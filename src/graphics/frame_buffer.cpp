//
// Created by skaldi on 06.02.26.
//

#include "graphics/frame_buffer.h"

#include "utility/gl_check.h"
#include "utility/gl_loader.h"
#include "graphics/render_states_cache.h"

namespace age
{
    template <typename F>
    struct scope_guard
    {
        F cleanup;
        ~scope_guard() { cleanup(); }
    };

    // Helper function to allow template argument deduction (C++17 style)
    template <typename F>
    scope_guard(F) -> scope_guard<F>;

    void frame_buffer::draw(render_target& target, texture& tex, const drawable& drawable_object, const render_states& states)
    {
        if (tex.get_id() == 0) return;
        if (!realize() || !bind()) return;

        scope_guard guard{[this](){ reset(); }};

        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.get_id(), 0));

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        target.draw(drawable_object, states);

        GL_CALL(glFinish());
    }

    void frame_buffer::draw(render_target& target, texture& tex, const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states)
    {
        if (tex.get_id() == 0) return;
        if (!realize() || !bind()) return;

        scope_guard guard{[this](){ reset(); }};

        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.get_id(), 0));

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        target.draw(vertices, num_vertices, indices, num_indices, states);

        GL_CALL(glFinish());
    }

    void frame_buffer::draw(render_target& target, texture& tex, const vertex_2d vertices[], size_t num_vertices, primitive_type type, const render_states& states)
    {
        if (tex.get_id() == 0) return;
        if (!realize() || !bind()) return;

        scope_guard guard{[this](){ reset(); }};

        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.get_id(), 0));

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        target.draw(vertices, num_vertices, type, states);

        GL_CALL(glFinish());
    }

    bool frame_buffer::realize()
    {
        if (m_handle == 0)
        {
            GLuint fbo{};

            GL_CALL(glGenFramebuffers(1, &fbo));

            m_handle = fbo;
        }

        return m_handle != 0;
    }

    bool frame_buffer::bind()
    {
        auto handle = get_handle();

        if (handle != g_render_state.framebuffer)
        {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, handle));

            g_render_state.framebuffer = handle;
        }

        return handle != 0;
    }

    void frame_buffer::reset()
    {
        if (g_render_state.framebuffer == m_handle)
        {
            g_render_state.framebuffer = 0;
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    }

    void frame_buffer::delete_handle(uint32_t handle)
    {
        GL_CALL(glDeleteFramebuffers(1, &handle));
        if (handle == g_render_state.framebuffer) g_render_state.framebuffer = 0;
    }
}

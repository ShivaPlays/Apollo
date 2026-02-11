#include "graphics/context.h"

#include <mutex>
#include <string>
#include <SDL3/SDL.h>
#include <stdexcept>

#include "graphics/render_window.h"
#include "utility/gl_loader.h"

namespace age
{
    void context::set_active(bool value)
    {
        if (value)
        {
            if (!is_active())
            {
                if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_render_window->get_internal_handle()), static_cast<SDL_GLContext>(m_GL_context.get())))
                {
                    throw std::runtime_error{ std::string{ "Failed to make context current\nSDL3 Error: " } + SDL_GetError() };
                }
            }
            return;
        }

        if (is_active())
        {
            if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_render_window->get_internal_handle()), nullptr))
            {
                throw std::runtime_error{ std::string{ "Failed to make context current\nSDL3 Error: " } + SDL_GetError() };
            }
        }
    }

    bool context::is_active() const
    {
        return SDL_GL_GetCurrentContext() == m_GL_context.get();
    }

    void context::flush() const
    {
        auto current_context = SDL_GL_GetCurrentContext();
        auto internal_window_handle = static_cast<SDL_Window*>(m_render_window->get_internal_handle());

        if (current_context != m_GL_context.get())
        {
            if (!SDL_GL_MakeCurrent(internal_window_handle, static_cast<SDL_GLContext>(m_GL_context.get())))
            {
                throw std::runtime_error{ std::string{ "Failed to make context current\nSDL Error: " } + SDL_GetError() };
            }
        }

        glFlush();

        if (current_context != m_GL_context.get())
        {
            if (!SDL_GL_MakeCurrent(internal_window_handle, current_context))
            {
                throw std::runtime_error{ std::string{ "Failed to make context current\nSDL Error: " } + SDL_GetError() };
            }
        }
    }

    void context::delete_context_lib(void* context)
    {
        SDL_GL_DestroyContext(static_cast<SDL_GLContext>(context));
    }

    void context::reset_GL_state() const
    {
        auto current_context = SDL_GL_GetCurrentContext();

        auto internal_window_handle = static_cast<SDL_Window*>(m_render_window->get_internal_handle());

        if (current_context != m_GL_context.get())
        {
            SDL_GL_MakeCurrent(internal_window_handle, static_cast<SDL_GLContext>(m_GL_context.get()));
        }

        // Unbind textures
        glBindTexture(GL_TEXTURE_2D, 0);

        // Unbind shader program
        glUseProgram(0);

        // Unbind VAO
        glBindVertexArray(0);

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Unbind buffer objects
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        if (current_context != m_GL_context.get())
        {
            SDL_GL_MakeCurrent(internal_window_handle, current_context);
        }
    }

    void context::create(const render_window& window, bool shared)
    {
#ifdef SDL_PLATFORM_ANDROID
        // Ask for OpenGL ES 3.0
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
        //Use OpenGL 4.3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
        m_render_window = &window;
        auto internal_window_handle = m_render_window->get_internal_handle();

        if (shared)
        {
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        }

        m_GL_context.reset(SDL_GL_CreateContext(static_cast<SDL_Window*>(internal_window_handle)));
        if (!m_GL_context)
        {
            throw std::runtime_error{ std::string{ "Failed to create context\nSDL Error: " } + SDL_GetError() };
        }

        if (!shared)
        {
            //Create the shared_context which we will use to share resources in threads
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            SDL_GLContext shared_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(internal_window_handle));
            if (!shared_context)
            {
                throw std::runtime_error{ std::string{ "Failed to create shared context\nSDL Error: " } + SDL_GetError() };
            }

            m_GL_shared_context.reset(shared_context);

            //SDL_GL_CreateContext makes the new context current. So lets reset it to the old one
            if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>(internal_window_handle), static_cast<SDL_GLContext>(m_GL_context.get())))
            {
                throw std::runtime_error{ std::string{ "Failed to make context current\nSDL Error: " } + SDL_GetError() };
            }
        }
    }

    std::shared_ptr<context> context::acquire_shared_context()
    {
        std::lock_guard<std::mutex> lock{ m_acquire_context_mutex };

        if (!m_GL_shared_context)
        {
            throw std::runtime_error{ std::string{ "Acquiring a shared context from a shared context is not possible. Acquire from main context!" }};
        }

        for (auto& v : m_shared_context_list)
        {
            if (v.use_count() == 1)
            {
                v->set_active(true);
                v->reset_GL_state();

                return v;
            }
        }

        auto current_context = SDL_GL_GetCurrentContext();

        //For testing in WayLand
        //if (!SDL_GL_MakeCurrent(nullptr, static_cast<SDL_GLContext>(m_GL_shared_context.get())))
        if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_render_window->get_internal_handle()), static_cast<SDL_GLContext>(m_GL_shared_context.get())))
        {
            throw std::runtime_error{ std::string{ "Failed to make context current\nSDL Error: " } + SDL_GetError() };
        }

        std::shared_ptr<context> shared_context;

        try
        {
            auto new_context = new context();
            shared_context.reset(new_context);
            shared_context->create(*m_render_window, true);
        }
        catch (...)
        {
            // Always restore context before rethrowing
            SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_render_window->get_internal_handle()), current_context);
            throw;
        }

        if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_render_window->get_internal_handle()), current_context))
        {
            throw std::runtime_error{ std::string{ "Failed to make context current\nSDL Error: " } + SDL_GetError() };
        }

        m_shared_context_list.push_back(shared_context);
        return shared_context;
    }
}

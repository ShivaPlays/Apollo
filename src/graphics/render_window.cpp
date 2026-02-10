#include "graphics/render_window.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>
//#include <SDL3/SDL_opengl.h>

#include <string>
#include <sstream>
#include <string_view>
#include <stdexcept>

#include "graphics/render_states_cache.h"
#include "utility/gl_check.h"

namespace age
{
	render_window::render_window()
		: m_clear_flags{ GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT }
	{
		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
		{
			std::stringstream warning;
			warning << "Warning: Unable to set Doublebuffer! SDL Error: " << SDL_GetError();

			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "%s", warning.str().c_str());
		}

		//m_windowhandle.reset(SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64, 64, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL));
		m_windowhandle.reset(SDL_CreateWindow("", 64, 64, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL));
		if (!m_windowhandle)
		{
			throw std::runtime_error{ std::string{ "Failed to create window\nSDL Error: " } + SDL_GetError() };
		}

		m_context.create(*this);

		//For testing purpose only as multithreeaded OpenGL has ome issues on Wayland
		//m_context.acquire_shared_context();

		//Use Vsync
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
			// Failed to initialize glad
			throw std::runtime_error{std::string{ "Failed to initialize GLAD" }};
		}

		init();
	}

	void render_window::open(const std::string_view& title, uint32_t display_index, uint32_t width, uint32_t height, uint32_t flags)
	{
		auto handle = static_cast<SDL_Window*>(m_windowhandle.get());

		SDL_SetWindowTitle(handle, title.data());
		SDL_SetWindowSize(handle , width, height);
		SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		if (flags & SDL_WINDOW_FULLSCREEN)
			SDL_SetWindowFullscreen(handle, SDL_WINDOW_FULLSCREEN);
		if (flags & SDL_WINDOW_BORDERLESS)
			SDL_SetWindowBordered(handle, false);
		if (flags & SDL_WINDOW_RESIZABLE)
			SDL_SetWindowResizable(handle, true);
		if (flags & SDL_WINDOW_MINIMIZED)
			SDL_MinimizeWindow(handle);
		if (flags & SDL_WINDOW_MAXIMIZED)
			SDL_MaximizeWindow(handle);
		if (flags & SDL_WINDOW_MOUSE_GRABBED)
		{
			SDL_SetWindowKeyboardGrab(handle, true);
			SDL_RaiseWindow(handle);
		}
		if (!(flags & SDL_WINDOW_HIDDEN))
			SDL_ShowWindow(handle);
		
		glm::vec2 view_size{ 1.0f, static_cast<float>(height) / static_cast<float>(width) };
		view_2d view{ view_size * 0.5f, view_size };

		apply_view(view);
	}

	void render_window::prepare_draw()
	{
		if (g_render_state.framebuffer != 0)
		{
			GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			g_render_state.framebuffer = 0;
			restore_view();
		}
	}

	const context& render_window::get_context() const
	{
		return m_context;
	}

	context& render_window::get_context()
	{
		return m_context;
	}

	void* render_window::get_internal_handle() const
	{
		return m_windowhandle.get();
	}

	void render_window::draw(const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states &states)
	{
		prepare_draw();
		render_target::draw(vertices, num_vertices, indices, num_indices, states);
	}

	void render_window::draw(const vertex_2d vertices[], size_t num_vertices, primitive_type type, const render_states& states)
	{
		prepare_draw();
		render_target::draw(vertices, num_vertices, type, states);
	}

	void render_window::clear()
	{
		prepare_draw();
		GL_CALL(glClear(m_clear_flags));
	}

	void render_window::display()
	{
		SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_windowhandle.get()));
	}

	glm::u32vec2 render_window::get_size() const
	{
		int w;
		int h;

		SDL_GetWindowSize(static_cast<SDL_Window*>(m_windowhandle.get()), &w, &h);

		return glm::u32vec2{ static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
	}

	int_rect render_window::get_safe_area() const
	{
		SDL_Rect area;
		SDL_GetWindowSafeArea(static_cast<SDL_Window*>(get_internal_handle()), &area);

		return int_rect{{area.x, area.y}, {area.w, area.h}};
	}

	void render_window::destroy_window_lib(void* window)
	{
		SDL_DestroyWindow(static_cast<SDL_Window*>(window));
	}


}
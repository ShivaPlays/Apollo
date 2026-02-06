#include "engine.h"

#define NO_SDL_GLEXT
#define GL_GLEXT_PROTOTYPES 1

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <array>

#include "utility/gl_check.h"

namespace age
{
	engine::engine()
		: m_initializer{ SDL_INIT_VIDEO }
		, m_started{ false }
		, m_exit_requested{ false }
	{
		static bool engine_instanced;

		if (engine_instanced)
		{
			throw std::runtime_error{ "Only one instance of engine is allowed!" };
		}

		m_instance = this;
		init_defaults();

		engine_instanced = true;
	}

	engine::~engine()
	{
		m_instance = nullptr;
	}

	engine::app_result engine::init(int argc, char* argv[])
	{
		return on_init(argc, argv);
	}

	void engine::start(std::string_view title, uint32_t display_index, uint32_t width, uint32_t height, uint32_t flags)
	{
		/*
		SDL_WINDOW_FULLSCREEN: fullscreen window
		SDL_WINDOW_FULLSCREEN_DESKTOP: fullscreen window at desktop resolution
		SDL_WINDOW_OPENGL: window usable with an OpenGL context
		SDL_WINDOW_VULKAN: window usable with a Vulkan instance
		SDL_WINDOW_METAL: window usable with a Metal instance
		SDL_WINDOW_HIDDEN: window is not visible
		SDL_WINDOW_BORDERLESS: no window decoration
		SDL_WINDOW_RESIZABLE: window can be resized
		SDL_WINDOW_MINIMIZED: window is minimized
		SDL_WINDOW_MAXIMIZED: window is maximized
		SDL_WINDOW_INPUT_GRABBED: window has grabbed input focus
		SDL_WINDOW_ALLOW_HIGHDPI: window should be created in high-DPI mode if supported (>= SDL 2.0.1)
		*/

		if (m_started)
		{
			throw std::runtime_error{ "Engine is already running." };
		}

		m_started = true;

		uint32_t sdl_flags = SDL_WINDOW_OPENGL;
		std::array<uint32_t, 7> own_flags{
			static_cast<uint32_t>(window_flags::fullscreen),
			static_cast<uint32_t>(window_flags::hidden),
			static_cast<uint32_t>(window_flags::borderless),
			static_cast<uint32_t>(window_flags::resizable),
			static_cast<uint32_t>(window_flags::minimized),
			static_cast<uint32_t>(window_flags::maximized),
			static_cast<uint32_t>(window_flags::input_grabbed),
		};

		std::array<uint32_t, 7> lib_flags{
			SDL_WINDOW_FULLSCREEN,
			SDL_WINDOW_HIDDEN,
			SDL_WINDOW_BORDERLESS,
			SDL_WINDOW_RESIZABLE,
			SDL_WINDOW_MINIMIZED,
			SDL_WINDOW_MAXIMIZED,
			SDL_WINDOW_MOUSE_GRABBED
		};

		for (size_t i = 0; i < own_flags.size(); ++i)
		{
			if (flags & own_flags[i])
				sdl_flags |= lib_flags[i];
		}

		m_render_window.open(title, display_index, width, height, sdl_flags);
		user_create();
	}

	void engine::stop()
	{
		m_exit_requested = true;
	}

	int32_t engine::init_lib(uint32_t flags)
	{
		return SDL_Init(flags);
	}

	void engine::quit_lib()
	{
		SDL_Quit();
	}

	void engine::init_defaults()
	{
		std::string_view vertex_shader_source =
			"#auto_version\n"
			"precision highp float;\n"
			"layout (std140) uniform viewprojection_matrix\n"
			"{\n"
			"	mat4 vp_m;\n"
			"};\n"
			"layout (std140) uniform model_matrix\n"
			"{\n"
			"	mat4 model_m;\n"
			"};\n"
			"layout (std140) uniform texture_matrices\n"
			"{\n"
			"	mat4 tex_m;\n"
			"};\n"
			"in vec2 a_position;\n"
			"in vec4 a_color;\n"
			"in vec2 a_uv;\n"
			"out vec4 v_color;\n"
			"out vec2 v_uv;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vp_m * model_m * vec4(a_position, 0.0, 1.0);\n"
			"	v_color = a_color;\n"
			"	vec4 t_coords = tex_m * vec4(a_uv, 0.0, 1.0);\n"
			"	v_uv = t_coords.xy;\n"
			"}";

		std::string_view fragment_shader_source =
			"#auto_version\n"
			"precision mediump float;\n"
			"uniform sampler2D u_texture;\n"
			"in vec4 v_color;\n"
			"in vec2 v_uv;\n"
			"out vec4 frag_color;\n"
			"void main()\n"
			"{\n"
			"	vec4 texel = texture(u_texture, v_uv);\n"
			"	//if(texel.a == 0.0) discard;\n"
			"	frag_color = v_color * texel;\n"
			"}";

		m_default_vertex_shader.compile(vertex_shader_source);
		m_default_fragment_shader.compile(fragment_shader_source);

		m_default_shader_program.attach_shader(m_default_vertex_shader);
		m_default_shader_program.attach_shader(m_default_fragment_shader);
		m_default_shader_program.bind_attrib_location(get_a_position_index(), "a_position");
		m_default_shader_program.bind_attrib_location(get_a_color_index(), "a_color");
		m_default_shader_program.bind_attrib_location(get_a_tex_coords_index(), "a_uv");
		m_default_shader_program.link();

		/*
		ToDo: later on gon with this approach. Have all the matrices separated and have 3 UBOs
		layout(std140) uniform MVP {
		mat4 model;
		mat4 view;
		mat4 projection;
		};

		layout(std140) uniform TexMatrix {
		mat4 textureMatrix;
		};

		layout(std140) uniform Viewport {
		vec4 viewport;
		};
		*/

		m_default_shader_program.set_uniform("u_texture", 0);
		m_default_shader_program.set_uniform_block_binding("viewprojection_matrix", get_vp_matrix_binding());
		m_default_shader_program.set_uniform_block_binding("model_matrix", get_model_matrix_binding());
		m_default_shader_program.set_uniform_block_binding("texture_matrices", get_texture_matrix_binding());

		m_default_texture.create(glm::u32vec2{ 1, 1 });
		m_default_texture.update(std::array<uint8_t, 4>{255, 255, 255, 255}.data());

		m_default_vertex_array_object.bind();
		m_default_vertex_buffer_object.bind();
		m_default_element_buffer_object.bind();

		m_vp_matrix_ubo.buffer_data(sizeof(glm::mat4x4), glm::value_ptr(glm::mat4{ 1.0f }));
		m_model_matrix_ubo.buffer_data(sizeof(glm::mat4x4), glm::value_ptr(glm::mat4{ 1.0f }));
		m_texture_matrix_ubo.buffer_data(sizeof(glm::mat4x4), glm::value_ptr(glm::mat4{ 1.0f }));
		
		m_viewport_ubo.buffer_data(sizeof(uint32_t) * 2, std::array<uint32_t, 2>{0, 0}.data());

		m_vp_matrix_ubo.bind_buffer_base(get_vp_matrix_binding());
		m_model_matrix_ubo.bind_buffer_base(get_model_matrix_binding());
		m_texture_matrix_ubo.bind_buffer_base(get_texture_matrix_binding());

		GL_CALL(glEnableVertexAttribArray(get_a_position_index()));
		GL_CALL(glEnableVertexAttribArray(get_a_color_index()));
		GL_CALL(glEnableVertexAttribArray(get_a_tex_coords_index()));

		GL_CALL(glVertexAttribPointer(get_a_position_index(), 2, GL_FLOAT, GL_FALSE, sizeof(vertex_2d), reinterpret_cast<void*>(0)));
		GL_CALL(glVertexAttribPointer(get_a_color_index(), 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_2d), reinterpret_cast<void*>(8)));
		GL_CALL(glVertexAttribPointer(get_a_tex_coords_index(), 2, GL_FLOAT, GL_FALSE, sizeof(vertex_2d), reinterpret_cast<void*>(12)));
		
		//m_default_vertex_array_object.release();
	}

	engine::app_result engine::user_create()
	{
		return on_user_create();
	}

	engine::app_result engine::update()
	{
		if (m_exit_requested) return app_result::exit_success;

		return on_update();
	}

	void engine::user_destroy()
	{
		on_user_destroy();
	}

	engine::app_result engine::process_event(SDL_Event& e)
	{
		return on_process_event(e);
	}

	engine::app_result engine::on_process_event(SDL_Event& e)
	{
		return app_result::exit_success;
	}
}
#pragma once

#include <string_view>

#include "graphics/render_window.h"
#include "graphics/vertex_array_object.h"
#include "graphics/vertex_buffer_object.h"
#include "graphics/texture.h"
#include "graphics/shader_program.h"
#include "graphics/uniform_buffer_object.h"
#include "graphics/vertex_array_object.h"
#include "graphics/vertex_buffer_object.h"
#include "utility/utility.h"

union SDL_Event;

namespace age
{
	namespace window_flags
	{
		enum flag
		{
			fullscreen =			1 << 0,
			fullscreen_desktop =	1 << 1,
			hidden =				1 << 2,
			borderless =			1 << 3,
			resizable =				1 << 4,
			minimized =				1 << 5,
			maximized =				1 << 6,
			input_grabbed =			1 << 7
		};
	}

	class engine
	{
	public:
		enum class app_result
		{
			keep_running,
			exit_success,
			exit_failure
		};

		friend app_result engine_init(age::engine& engine, int argc, char** argv);
		friend app_result engine_update(engine& engine);
		friend app_result engine_process_event(engine& engine, SDL_Event* event);
		friend void engine_quit(engine& engine);

		engine();
		virtual ~engine();

		engine(const engine& other) = delete;
		engine(engine&& other) = delete;

		engine& operator = (const engine& other) = delete;
		engine& operator = (engine&& other) = delete;

	public:

		void start(std::string_view title, uint32_t display_index, uint32_t width, uint32_t height, uint32_t flags);
		void stop();

		const render_window& get_render_window() const { return m_render_window; }
		render_window& get_render_window() { return m_render_window; }

		const vertex_array_object& get_default_vertex_array_object() const { return m_default_vertex_array_object; }
		const vertex_buffer_object& get_default_vertex_buffer_object() const { return m_default_vertex_buffer_object; }
		const vertex_buffer_object& get_default_element_buffer_object() const { return m_default_element_buffer_object; }

		vertex_array_object& get_default_vertex_array_object() { return m_default_vertex_array_object; }
		vertex_buffer_object& get_default_vertex_buffer_object() { return m_default_vertex_buffer_object; }
		vertex_buffer_object& get_default_element_buffer_object() { return m_default_element_buffer_object; }

		const uniform_buffer_object& get_vp_matrix_ubo() const{ return m_vp_matrix_ubo; }
		const uniform_buffer_object& get_model_matrix_ubo() const { return m_model_matrix_ubo; }
		const uniform_buffer_object& get_texture_matrix_ubo() const { return m_texture_matrix_ubo; }
		const uniform_buffer_object& get_viewport_uniform_buffer_object() const { return m_viewport_ubo; }
		uniform_buffer_object& get_vp_matrix_ubo() { return m_vp_matrix_ubo; }
		uniform_buffer_object& get_model_matrix_ubo() { return m_model_matrix_ubo; }
		uniform_buffer_object& get_texture_matrix_ubo() { return m_texture_matrix_ubo; }
		uniform_buffer_object& get_viewport_uniform_buffer_object() { return m_viewport_ubo; }

		const shader& get_default_vertex_shader() const { return m_default_vertex_shader; }
		const shader& get_default_fragment_shader() const { return m_default_fragment_shader; }
		const shader_program& get_default_shader_program() const { return m_default_shader_program; }
		const texture& get_default_texture() const { return m_default_texture; }

		static engine* get_instance() { return m_instance; }

		static constexpr uint32_t get_a_position_index() { return 0; }
		static constexpr uint32_t get_a_color_index() { return 1; }
		static constexpr uint32_t get_a_tex_coords_index() { return 2; }

		static constexpr uint32_t get_vp_matrix_binding() { return 0; }
		static constexpr uint32_t get_model_matrix_binding() { return 1; }
		static constexpr uint32_t get_texture_matrix_binding() { return 2; }

		static void set_mouse_touch_events(bool value);
		static bool get_mouse_touch_events();

		static bool is_device_reset() { return m_device_reset; }

	protected:

	private:
		virtual app_result on_init(int argc, char* argv[]) = 0;
		virtual app_result on_user_create() = 0;
		virtual app_result on_update() = 0;
		virtual void on_user_destroy() = 0;
		virtual app_result on_process_event(SDL_Event& e);

		static int32_t init_lib(uint32_t flags);
		static void quit_lib();

		static void set_device_reset(bool value) { m_device_reset = value; }
		static void reset_render_cache();

		void init_defaults();

		app_result init(int argc, char* argv[]);
		app_result user_create();
		app_result update();
		void user_destroy();
		app_result process_event(SDL_Event& e);

		inline static engine* m_instance = nullptr;
		inline static bool m_device_reset = false;

		initializer<init_lib, quit_lib, uint32_t> m_initializer;
		render_window m_render_window;

		vertex_array_object m_default_vertex_array_object;
		vertex_buffer_object m_default_vertex_buffer_object{ vertex_buffer_object::target::array };
		vertex_buffer_object m_default_element_buffer_object{ vertex_buffer_object::target::element_array };

		uniform_buffer_object m_vp_matrix_ubo;
		uniform_buffer_object m_model_matrix_ubo;
		uniform_buffer_object m_texture_matrix_ubo;
		uniform_buffer_object m_viewport_ubo;
		shader m_default_vertex_shader{ shader::shader_type::vertex };
		shader m_default_fragment_shader{ shader::shader_type::fragment };
		shader_program m_default_shader_program;
		texture m_default_texture;

		bool m_started;
		bool m_exit_requested;
	};

	typedef engine application;
}
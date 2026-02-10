#include "graphics/vertex_array_object.h"

#include <glad/glad.h>

#include "graphics/render_states_cache.h"
#include "utility/gl_check.h"
#include "engine.h"

namespace age
{
	vertex_array_object::vertex_array_object()
	{}

	bool vertex_array_object::bind()
	{
		if (!m_handle) m_handle = create_handle();

		auto handle = get_handle();

		if (g_render_state.vertex_array != handle)
		{
			GL_CALL(glBindVertexArray(get_handle()));

			g_render_state.vertex_array = handle;
		}

		return handle != 0;
	}

	void vertex_array_object::release() const
	{
		auto handle = get_handle();

		if (g_render_state.vertex_array == handle)
		{
			GL_CALL(glBindVertexArray(0));

			g_render_state.vertex_array = 0;
		}
	}

	void vertex_array_object::invalidate()
	{
		delete_handle(get_handle());

		m_handle = 0;
	}

	void vertex_array_object::reset_cache()
	{
		g_render_state.vertex_array = 0;
	}

	uint32_t vertex_array_object::create_handle()
	{
		GLuint handle;

		GL_CALL(glGenVertexArrays(1, &handle));

		return handle;
	}

	void vertex_array_object::delete_handle(uint32_t handle)
	{
		if (handle && !engine::is_device_reset()) GL_CALL(glDeleteVertexArrays(1, &handle));
		if (handle == g_render_state.vertex_array) g_render_state.vertex_array = 0;
	}
}
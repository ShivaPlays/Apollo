#include "graphics/uniform_buffer_object.h"

#include <glad/glad.h>

#include "graphics/render_states_cache.h"
#include "utility/gl_check.h"
#include "engine.h"

namespace age
{
	bool uniform_buffer_object::bind() const
	{
		auto handle = get_handle();

		if (handle != g_render_state.uniform_buffer_object)
		{
			GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, handle));

			g_render_state.uniform_buffer_object = handle;
		}

		return handle != 0;
	}

	void uniform_buffer_object::buffer_data(size_t size, const void* data)
	{
		create_handle();

		if (bind())
			GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW));
	}

	void uniform_buffer_object::buffer_sub_data(size_t offset, size_t size, const void* data)
	{
		if (bind())
			GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	}

	void uniform_buffer_object::bind_buffer_base(uint32_t index)
	{
		create_handle();

		GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, index, get_handle()));
	}

	void uniform_buffer_object::bind_buffer_range(uint32_t index, size_t offset, size_t size)
	{
		create_handle();

		GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, index, get_handle(), offset, size));
	}

	void uniform_buffer_object::invalidate()
	{
		delete_handle(get_handle());
		m_handle = 0;
	}

	void uniform_buffer_object::reset_cache()
	{
		g_render_state.uniform_buffer_object = 0;
	}

	void uniform_buffer_object::create_handle()
	{
		if (!m_handle) m_handle = gen_handle();
	}

	uint32_t uniform_buffer_object::gen_handle()
	{
		GLuint handle;

		GL_CALL(glGenBuffers(1, &handle));

		return handle;
	}

	void uniform_buffer_object::delete_handle(uint32_t handle)
	{
		if (handle && !engine::is_device_reset()) GL_CALL(glDeleteBuffers(1, &handle));
		if (handle == g_render_state.uniform_buffer_object) g_render_state.uniform_buffer_object = 0;
	}
}
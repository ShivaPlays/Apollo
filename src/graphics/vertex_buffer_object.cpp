#include "graphics/vertex_buffer_object.h"

#include <glad/glad.h>

#include <stdexcept>

#include "graphics/render_states_cache.h"
#include "utility/gl_check.h"
#include "engine.h"

namespace age
{
	vertex_buffer_object::vertex_buffer_object(target target)
		: m_handle{ create_handle() }
		, m_target{ target }
	{}

	bool vertex_buffer_object::bind() const
	{
		auto handle = get_handle();
		auto& current_bound_buffer = g_render_state.vertex_buffer_objects[static_cast<uint32_t>(m_target)];

		if (current_bound_buffer != handle)
		{
			GL_CALL(glBindBuffer(convert_target(m_target), get_handle()));

			current_bound_buffer = handle;
		}

		return current_bound_buffer != 0;
	}

	void vertex_buffer_object::buffer_data(const void* data, size_t size_in_bytes, usage usage)
	{
		if (!m_handle) m_handle = create_handle();

		if (bind())
		{
			GL_CALL(glBufferData(convert_target(m_target), size_in_bytes, data, convert_usage(usage)));

			m_last_buffer_size[static_cast<uint32_t>(m_target)] = size_in_bytes;
			m_last_buffer_usage[static_cast<uint32_t>(m_target)] = usage;
		}
	}

	void vertex_buffer_object::update_data(const void* data, size_t size_in_bytes, usage usage)
	{
		if (!m_handle) m_handle = create_handle();

		if (bind())
		{
			auto target = convert_target(m_target);

			GL_CALL(glBufferData(target, m_last_buffer_size[static_cast<uint32_t>(m_target)], nullptr, convert_usage(m_last_buffer_usage[static_cast<uint32_t>(m_target)])));
			GL_CALL(glBufferData(target, size_in_bytes, data, convert_usage(usage)));

			m_last_buffer_size[static_cast<uint32_t>(m_target)] = size_in_bytes;
			m_last_buffer_usage[static_cast<uint32_t>(m_target)] = usage;
		}
	}

	void vertex_buffer_object::buffer_sub_data(const void* data, size_t offset, size_t size_in_bytes)
	{
		if (bind())
		{
			GL_CALL(glBufferSubData(convert_target(m_target), offset, size_in_bytes, data));
		}
	}

	void vertex_buffer_object::invalidate()
	{
		delete_handle(get_handle());

		auto& current_bound_buffer = g_render_state.vertex_buffer_objects[static_cast<uint32_t>(m_target)];

		m_handle = 0;
		current_bound_buffer = 0;
	}

	void vertex_buffer_object::reset_cache()
	{
		g_render_state.vertex_buffer_objects.fill(0);
	}

	uint32_t vertex_buffer_object::convert_target(target target_to_convert)
	{
		switch (target_to_convert)
		{
			case target::array:
				return GL_ARRAY_BUFFER;
			case target::element_array:
				return GL_ELEMENT_ARRAY_BUFFER;
			default:
				throw std::runtime_error{ "VERTEX_BUFFER_OBJECT::CONVERT_TARGET INVALID TARGET!" };
		}
	}

	uint32_t vertex_buffer_object::convert_usage(usage usage_to_convert)
	{
		switch (usage_to_convert)
		{
			case usage::stream_draw:
				return GL_STREAM_DRAW;
			case usage::static_draw:
				return GL_STATIC_DRAW;
			case usage::dynamic_draw:
				return GL_DYNAMIC_DRAW;
			default:
				throw std::runtime_error{ "VERTEX_BUFFER_OBJECT::CONVERT_USAGE INVALID USAGE!" };
		}
	}

	uint32_t vertex_buffer_object::create_handle()
	{
		GLuint handle{};
		GL_CALL(glGenBuffers(1, &handle));

		return handle;
	}

	void vertex_buffer_object::delete_handle(uint32_t handle)
	{
		if (handle && !engine::is_device_reset()) GL_CALL(glDeleteBuffers(1, &handle));
		for (auto &b : g_render_state.vertex_buffer_objects)
		{
			if (b == handle)
			{
				b = 0;
				break;
			}
		}
	}
}
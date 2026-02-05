#include "graphics/shader.h"

#include <glad/glad.h>

#include <stdexcept>
#include <SDL3/SDL.h>

#include "utility/gl_check.h"

namespace age
{
	shader::shader(shader_type type)
		: m_type{ type }
		, m_handle{ GL_CALL(glCreateShader(convert_type(type))) }
	{
		if (!m_handle)
		{
			throw std::runtime_error{ "Error creating shader" };
		}
	}

	void shader::compile(std::string_view shader_source)
	{
		const GLchar* data = shader_source.data();

		GL_CALL(glShaderSource(m_handle, 1, &data, nullptr));
		GL_CALL(glCompileShader(m_handle));

		GLint success;
		GL_CALL(glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			static constexpr size_t logSize = 512;
			char infoLog[logSize];

			GL_CALL(glGetShaderInfoLog(m_handle, logSize, nullptr, infoLog));
			throw std::runtime_error{ std::string{ "ERROR::SHADER::COMPILATION_FAILED\n" } + infoLog };
		};

		GLint log_length;
		GL_CALL(glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &log_length));
		if (log_length)
		{
			static constexpr size_t logSize = 512;
			char infoLog[logSize];

			GL_CALL(glGetShaderInfoLog(m_handle, logSize, nullptr, infoLog));
			SDL_Log("%s", infoLog);
		}
	}

	uint32_t shader::convert_type(shader_type type_to_convert)
	{
		switch (type_to_convert)
		{
		case shader_type::fragment:
			return GL_FRAGMENT_SHADER;

		case shader_type::geometry:
			return GL_GEOMETRY_SHADER;

		case shader_type::vertex:
			return GL_VERTEX_SHADER;

		default:
			throw std::runtime_error{ "SHADER::CONVERT_TYPE INVALID SHADER_TYPE!" };
		}
	}

	void shader::delete_handle(uint32_t handle)
	{
		GL_CALL(glDeleteShader(handle));
	}
}
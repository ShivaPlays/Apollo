#include "graphics/shader.h"

#include "utility/gl_loader.h"

#include <stdexcept>
#include <SDL3/SDL.h>

#include "utility/gl_check.h"
#include "engine.h"

namespace age
{
	shader::shader(shader_type type)
		: m_type{ type }
	{}

	void shader::compile(std::string_view shader_source)
	{
		if (!m_handle)
		{
			m_handle = GL_CALL(glCreateShader(convert_type(m_type)));
			if (!m_handle) throw std::runtime_error{ "Error creating shader" };
		}

        const GLchar* header = "\0";
		const GLchar* data = shader_source.data();

        auto auto_version_length = AUTO_VERSION.length();

        if (shader_source.size() >= auto_version_length &&
            shader_source.compare(0, auto_version_length, AUTO_VERSION) == 0)
        {
#ifdef SDL_PLATFORM_ANDROID
            auto new_header = EGL_HEADER;
#else
        	auto new_header = OGL_HEADER;
#endif
            header = new_header.data();
            data = &shader_source.at(auto_version_length);
        }

        const GLchar* sources[2] = {header, data};

		GL_CALL(glShaderSource(m_handle, 2, sources, nullptr));
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

	void shader::invalidate()
	{
		delete_handle(m_handle);
		m_handle = 0;
	}

	uint32_t shader::convert_type(shader_type type_to_convert)
	{
		switch (type_to_convert)
		{
		case shader_type::fragment:
			return GL_FRAGMENT_SHADER;

		case shader_type::vertex:
			return GL_VERTEX_SHADER;

		default:
			throw std::runtime_error{ "SHADER::CONVERT_TYPE INVALID SHADER_TYPE!" };
		}
	}

	void shader::delete_handle(uint32_t handle)
	{
		if (handle != 0 && !engine::is_device_reset()) GL_CALL(glDeleteShader(handle));
	}
}
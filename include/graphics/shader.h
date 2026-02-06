#pragma once

#include <string_view>
#include <cstdint>

#include "../utility/utility.h"

namespace age
{
	class shader
	{
	public:
		friend class shader_program;

		enum class shader_type
		{
			vertex,
			geometry,
			fragment
		};

		shader(shader_type shader_type);
		
	public:
		shader_type get_type() const { return m_type; }
		void compile(std::string_view shader_source);

	protected:

	private:

        static constexpr std::string_view AUTO_VERSION = "#auto_version";
        static constexpr std::string_view OGL_HEADER = "#version 430 core";
        static constexpr std::string_view EGL_HEADER = "#version 300 es";

		static uint32_t convert_type(shader_type type_to_convert);
		static void delete_handle(uint32_t handle);

		uint32_t get_handle() const { return m_handle; }

		unique_handle<uint32_t, delete_handle> m_handle;

		shader_type m_type;
	};
}
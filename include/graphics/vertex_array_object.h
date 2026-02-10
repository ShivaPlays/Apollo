#pragma once

#include "../utility/utility.h"

#include <cstdint>

namespace age
{
	class vertex_array_object
	{
	public:
		vertex_array_object();
	public:
		bool bind();
		void release() const;

		void invalidate();
		static void reset_cache();
	protected:

	private:
		uint32_t get_handle() const { return m_handle; }

		static uint32_t create_handle();
		static void delete_handle(uint32_t handle);
		unique_handle<uint32_t, delete_handle> m_handle;
	};
}
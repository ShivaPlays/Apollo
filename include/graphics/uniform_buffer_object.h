#pragma once

#include "utility/utility.h"
#include <cstdint>

namespace age
{
	class uniform_buffer_object
	{
	public:
		uniform_buffer_object() = default;
	public:
		bool bind() const;

		void buffer_data(size_t size, const void* data);
		void buffer_sub_data(size_t offset, size_t size, const void* data);

		void bind_buffer_base(uint32_t index);
		void bind_buffer_range(uint32_t index, size_t offset, size_t size);

		void invalidate();

		static bool bind(const uniform_buffer_object* ubo);
		static void reset_cache();
	protected:

	private:
		void create_handle();

		static uint32_t gen_handle();
		static void delete_handle(uint32_t handle);

		uint32_t get_handle() const { return m_handle; };

		unique_handle<uint32_t, delete_handle> m_handle;
	};
}
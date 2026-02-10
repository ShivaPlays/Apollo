#pragma once

#include <cstdint>
#include <array>

#include "../utility/utility.h"

namespace age
{
	class vertex_buffer_object
	{
	public:
		enum class target : uint32_t
		{
			array = 0,
			element_array,

			num_elements
		};

		enum class usage : uint32_t
		{
			stream_draw,
			static_draw,
			dynamic_draw
		};

		vertex_buffer_object(target target);

	public:
		void set_target(target value) noexcept { m_target = value; }
		target get_target() const noexcept{ return m_target; }

		bool bind() const;
		
		void buffer_data(const void* data, size_t size_in_bytes, usage usage);
		void update_data(const void* data, size_t size_in_bytes, usage usage);
		void buffer_sub_data(const void* data, size_t offset, size_t size_in_bytes);

		void invalidate();
		static void reset_cache();
	protected:

	private:
		static uint32_t convert_target(target target_to_convert);
		static uint32_t convert_usage(usage usage_to_convert);

		uint32_t get_handle() const { return m_handle; }

		static uint32_t create_handle();
		static void delete_handle(uint32_t handle);
		unique_handle<uint32_t, delete_handle> m_handle;

		target m_target;

		std::array<size_t, static_cast<uint32_t>(target::num_elements)> m_last_buffer_size{0};
		std::array<usage, static_cast<uint32_t>(target::num_elements)> m_last_buffer_usage{usage::static_draw};

		//size_t m_last_buffer_size{};
		//usage m_last_buffer_usage = usage::static_draw;
	};
}
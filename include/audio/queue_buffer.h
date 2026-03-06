#pragma once

#include <stdint.h>

#include "buffer.h"

namespace age::audio
{
	class source;

	class queue_buffer
	{
	public:
		friend class source;

		queue_buffer(uint32_t handle);
		queue_buffer(const buffer& buffer);

	public:
		void buffer_data(buffer::format the_format, const std::byte data[], size_t size_in_bytes, uint32_t frequency);

	protected:

	private:
		uint32_t get_handle() const { return m_handle; }

		uint32_t m_handle{};
	};
}
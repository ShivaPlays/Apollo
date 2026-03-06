#include "audio/queue_buffer.h"

#include <AL/al.h>

#include <utility/al_check.h>

namespace age::audio
{
	queue_buffer::queue_buffer(uint32_t handle)
		: m_handle{ handle }
	{}

	queue_buffer::queue_buffer(const buffer& buffer)
		: m_handle{ buffer.get_handle() }
	{}

	void queue_buffer::buffer_data(buffer::format the_format, const std::byte data[], size_t size_in_bytes, uint32_t frequency)
	{
		AL_CALL(alBufferData(m_handle, buffer::format_to_AL_enum(the_format), data, static_cast<ALsizei>(size_in_bytes), frequency));
	}
}
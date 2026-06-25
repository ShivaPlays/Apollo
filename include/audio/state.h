#pragma once

#include <cstdint>

namespace age::audio
{
	enum class state
	{
		playing,
		paused,
		stopped
	};

	state i_to_state(uint32_t value);
}

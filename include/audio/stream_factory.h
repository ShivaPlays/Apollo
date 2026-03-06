#pragma once

#include <memory>
#include <istream>

#include "stream.h"

namespace age::audio::stream_factory
{
	std::unique_ptr<stream> create_from_stream(std::istream& is);
}
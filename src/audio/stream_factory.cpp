#include "audio/stream_factory.h"

#include "audio/format.h"
#include "audio/priv/ogg_stream.h"

namespace age::audio::stream_factory
{
	std::unique_ptr<stream> create_from_stream(std::istream& is)
	{
		if (get_format(is) == format::ogg)
		{
			//ToDo add propert Test if stream is for an ogg file
		}

		std::unique_ptr<stream> result = std::make_unique<ogg_stream>();
		return result;
	}
}
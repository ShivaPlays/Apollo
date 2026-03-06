#include "audio/format.h"

#include "audio/wave.h"
#include "utility/endian.h"

namespace age::audio
{
	format get_format(std::istream& is)
	{
		auto position = is.tellg();
		file::wave::header wave_h;
		char buffer[4] = { 0, 0, 0, 0 };

		//peek a bit into the stream to determin with what filetype we are dealing
		is.read(reinterpret_cast<char*>(wave_h.RIFF), sizeof(decltype(wave_h.RIFF)));
		is.read(buffer, sizeof(decltype(buffer)));
		is.read(reinterpret_cast<char*>(wave_h.WAVE), sizeof(decltype(wave_h.WAVE)));
		is.seekg(position);

		if (wave_h.RIFF[0] == 'R' && wave_h.RIFF[1] == 'I' && wave_h.RIFF[2] == 'F' && wave_h.RIFF[3] == 'F'
			&& wave_h.WAVE[0] == 'W' && wave_h.WAVE[1] == 'A' && wave_h.WAVE[2] == 'V' && wave_h.WAVE[3] == 'E')
		{
			return format::wave;
		}

		return format::unknown;
	}

	format get_format(const std::byte data[], size_t size_in_bytes)
	{
		if (!data || !size_in_bytes)
			return format::unknown;

		if (size_in_bytes >= sizeof(file::wave::header))
		{
			file::wave::header header{};

			size_t cursor = 0;
			for (cursor = 0; cursor < 4; ++cursor)
				header.RIFF[cursor] = std::to_integer<uint8_t>(data[cursor]);

			header.chunk_size = endian::convert_to_int(reinterpret_cast<const std::byte*>(&data[cursor]), 4);
			cursor += 4;

			for (size_t i = 0; i < 4; ++i)
				header.WAVE[i] = std::to_integer<uint8_t>(data[cursor++]);

			if (header.RIFF[0] == 'R' && header.RIFF[1] == 'I' && header.RIFF[2] == 'F' && header.RIFF[3] == 'F'
				&& header.WAVE[0] == 'W' && header.WAVE[1] == 'A' && header.WAVE[2] == 'V' && header.WAVE[3] == 'E')
			{
				return format::wave;
			}
		}

		return format::unknown;
	}
}
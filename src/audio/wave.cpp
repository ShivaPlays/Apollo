#include "audio/wave.h"

#include "system/asset_istream.h"
#include "utility/endian.h"

namespace age::audio::file
{
	void wave::load(std::string_view fn)
	{
		asset_istream is{ fn.data(), std::ios::binary };
		load(is);
	}

	void wave::load(std::istream& is)
	{
		auto error = []() -> void { throw std::runtime_error{ "Error reading wave file" }; };
		char buffer[4] = { 0, 0, 0, 0 };
		m_header = header{};
		std::vector<std::byte> new_data;
		m_data.swap(new_data);
		new_data.clear();
		header new_header;

		if (is.read(reinterpret_cast<char*>(new_header.RIFF), sizeof(new_header.RIFF)).gcount() != sizeof(new_header.RIFF))
			error();

		if (is.read(buffer, 4).gcount() != 4)
			error();

		new_header.chunk_size = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 4);

		if (is.read(reinterpret_cast<char*>(new_header.WAVE), sizeof(new_header.WAVE)).gcount() != sizeof(new_header.WAVE))
			error();

		if (is.read(reinterpret_cast<char*>(new_header.fmt), sizeof(new_header.fmt)).gcount() != sizeof(new_header.fmt))
			error();

		if (is.read(buffer, 4).gcount() != 4)
			error();

		new_header.subchunk1_size = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 4);

		if (is.read(buffer, 2).gcount() != 2)
			error();

		new_header.audio_format = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 2);

		if (is.read(buffer, 2).gcount() != 2)
			error();

		new_header.num_of_chan = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 2);

		if (is.read(buffer, 4).gcount() != 4)
			error();

		new_header.samples_per_sec = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 4);

		if (is.read(buffer, 4).gcount() != 4)
			error();

		new_header.bytes_per_sec = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 4);

		if (is.read(buffer, 2).gcount() != 2)
			error();

		new_header.block_align = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 2);

		if (is.read(buffer, 2).gcount() != 2)
			error();

		new_header.bits_per_sample = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 2);

		if (is.read(reinterpret_cast<char*>(new_header.data_id), sizeof(new_header.data_id)).gcount() != sizeof(new_header.data_id))
			error();

		if (is.read(buffer, 4).gcount() != 4)
			error();

		new_header.data_size = endian::convert_to_int(reinterpret_cast<std::byte*>(buffer), 4);

		if (!new_header.data_size)
			error();

		new_data.resize(new_header.data_size);
		if (is.read(reinterpret_cast<char*>(&new_data[0]), new_header.data_size).gcount() != new_header.data_size)
			error();

		m_data.swap(new_data);
		m_header = new_header;
	}

	void wave::load(const std::byte data[], size_t size_in_bytes)
	{
		char buffer[4]{ 0, 0, 0, 0 };
		m_header = header{};
		m_data.clear();

	}

	const wave::header& wave::get_header() const
	{
		return m_header;
	}

	const std::vector<std::byte>& wave::get_data() const
	{
		return m_data;
	}
}
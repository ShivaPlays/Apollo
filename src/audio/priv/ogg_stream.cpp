#include "audio/priv/ogg_stream.h"

#include <stdexcept>

size_t read(void* ptr, size_t size, size_t nmemb, void* data)
{
	std::istream* stream = static_cast<std::istream*>(data);
	size_t bytes_to_read = size * nmemb;

	auto result = static_cast<size_t>(stream->read(reinterpret_cast<char*>(ptr), bytes_to_read).gcount());

	return result;
}

int seek(void* data, ogg_int64_t offset, int whence)
{
	std::istream* stream = static_cast<std::istream*>(data);
    switch (whence)
    {
    case SEEK_SET:
		stream->seekg(offset, std::ios_base::beg);
        break;
    case SEEK_CUR:
		stream->seekg(offset, std::ios_base::cur);
        break;
    case SEEK_END:
		stream->seekg(-offset, std::ios_base::end);
		break;
    }

	auto eof = stream->eof();
	auto fail = stream->fail();
	auto pos = stream->tellg();

	return static_cast<int>(stream->tellg());
}

long tell(void* data)
{
	std::istream* stream = static_cast<std::istream*>(data);
	return static_cast<long>(stream->tellg());
}

static ov_callbacks callbacks = { &read, &seek, NULL, &tell };

namespace age::audio::priv
{
	ogg_stream::~ogg_stream()
	{
		close();
	}

	void ogg_stream::close()
	{
		if (!m_vorbis_file.datasource) return;

		ov_clear(&m_vorbis_file);
		m_vorbis_file.datasource = nullptr;
		m_channel_count = 0;
	}

	stream::info ogg_stream::on_open(std::istream& is)
	{
		int status = ov_open_callbacks(&is, &m_vorbis_file, nullptr, 0, callbacks);
		if (status < 0)
		{
			throw std::runtime_error{ "Error opening Vorbis file for reading" };
		}

		info result;
		vorbis_info* v_info = ov_info(&m_vorbis_file, -1);

		result.channel_count = v_info->channels;
		result.sample_rate = v_info->rate;
		result.sample_count = ov_pcm_total(&m_vorbis_file, -1) * v_info->channels;

		m_channel_count = result.channel_count;
		m_istream = &is;

		return result;
	}

	void ogg_stream::on_seek(uint64_t sample_offset)
	{
		ov_pcm_seek(&m_vorbis_file, sample_offset / m_channel_count);
	}

	size_t ogg_stream::on_read(std::byte samples[], size_t max_count)
	{
		size_t count = 0;

		while (count < max_count)
		{
			size_t bytes_to_read = max_count - count;

			auto bytes_read = ov_read(&m_vorbis_file, reinterpret_cast<char*>(samples), static_cast<int>(bytes_to_read), 0, 2, 1, nullptr);

			if (bytes_read <= 0) break;

			count += bytes_read;
			samples += bytes_read;
		}

		return count;
	}

	void ogg_stream::on_reset()
	{
		if (!m_istream) return;

		m_istream->clear();
		seek(0);
	}
}
#pragma once

#include "../stream.h"

#include <vorbis/vorbisfile.h>

namespace age::audio::priv
{
	class ogg_stream
		: public stream
	{
	public:
		ogg_stream() = default;
		ogg_stream(const ogg_stream& other) = delete;
		ogg_stream(ogg_stream&& other) = delete;

		ogg_stream& operator = (const ogg_stream& other) = delete;
		ogg_stream& operator = (ogg_stream&& other) = delete;

		virtual ~ogg_stream() override;

	public:
		void close();
		
	protected:
		virtual info on_open(std::istream& is) override;
		virtual void on_seek(uint64_t sample_offset) override;
		virtual size_t on_read(std::byte samples[], size_t max_count) override;
		virtual void on_reset() override;

	private:

		OggVorbis_File m_vorbis_file;
		uint32_t m_channel_count{};

		std::istream* m_istream{};
	};
}
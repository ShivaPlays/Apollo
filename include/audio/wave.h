#pragma once

#include <stdint.h>
#include <string_view>
#include <istream>
#include <vector>

namespace age::audio::file
{
	class wave
	{
	public:
		struct header
		{
			header()
				: RIFF{}
				, chunk_size{}
				, WAVE{}
				, fmt{}
				, subchunk1_size{}
				, audio_format{}
				, num_of_chan{}
				, samples_per_sec{}
				, bytes_per_sec{}
				, block_align{}
				, bits_per_sample{}
				, data_id{}
				, data_size{}
			{}

			/* RIFF Chunk Descriptor */
			uint8_t         RIFF[4];			// RIFF Header Magic header
			uint32_t        chunk_size;			// RIFF Chunk Size
			uint8_t         WAVE[4];			// WAVE Header
			/* "fmt" sub-chunk */
			uint8_t         fmt[4];				// FMT header
			uint32_t        subchunk1_size;		// Size of the fmt chunk
			uint16_t        audio_format;		// Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
			uint16_t        num_of_chan;		// Number of channels 1=Mono 2=Sterio
			uint32_t        samples_per_sec;	// Sampling Frequency in Hz
			uint32_t        bytes_per_sec;		// bytes per second
			uint16_t        block_align;		// 2=16-bit mono, 4=16-bit stereo
			uint16_t        bits_per_sample;	// Number of bits per sample
			/* "data" sub-chunk */
			uint8_t         data_id[4];			// "data"  string
			uint32_t        data_size;			// Sampled data length
		};

	public:
		void load(std::string_view fn);
		void load(std::istream& is);
		void load(const std::byte data[], size_t size_in_bytes);

		const header& get_header() const;
		const std::vector<std::byte>& get_data() const;

	protected:

	private:

		header m_header;
		std::vector<std::byte> m_data;
	};
}
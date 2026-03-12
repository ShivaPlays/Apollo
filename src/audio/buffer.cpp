#include "audio/buffer.h"

#include <AL/al.h>

#include <array>
#include <type_traits>
#include <stdexcept>

#include "audio/device.h"
#include "audio/format.h"
#include "audio/wave.h"
#include "system/assetstream.h"

#include "audio/priv/al_check.h"

namespace age::audio
{
	buffer::~buffer()
	{
		if (m_handle)
			device::get().remove_buffer_from_active_sources(*this);
	}

	buffer::buffer(buffer&& other) noexcept
		: m_handle{std::exchange(other.m_handle, 0)}
	{}

	buffer& buffer::operator=(buffer&& other) noexcept
	{
		if (this == &other) return *this;

		this->m_handle = std::exchange(other.m_handle, 0);

		return *this;
	}

	void buffer::load(std::string_view fn)
	{
		assetistream is{ fn.data(), std::ios::binary };
		load(is);
	}

	void buffer::load(std::istream& is)
	{
		switch (get_format(is))
		{
			case audio::format::wave:
			{
				file::wave wave_file;
				wave_file.load(is);

				format the_format = format::mono_8;

				if (wave_file.get_header().num_of_chan == 1 && wave_file.get_header().bits_per_sample == 8)
					the_format = format::mono_8;
				else if (wave_file.get_header().num_of_chan == 1 && wave_file.get_header().bits_per_sample == 16)
					the_format = format::mono_16;
				else if (wave_file.get_header().num_of_chan == 2 && wave_file.get_header().bits_per_sample == 8)
					the_format = format::stereo_8;
				else if (wave_file.get_header().num_of_chan == 2 && wave_file.get_header().bits_per_sample == 16)
					the_format = format::stereo_16;
				else
					throw std::runtime_error{ "Unrecognised wave format" };

				buffer_data(the_format, wave_file.get_data().data(), wave_file.get_data().size(), wave_file.get_header().samples_per_sec);
			}
			break;

			default:
			{
				throw std::runtime_error{ "Not supported format" };
			}
			break;
		}
	}

	void buffer::load(std::byte data[], size_t size_in_bytes)
	{

		switch (get_format(data, size_in_bytes))
		{
			default:
			{
				throw std::runtime_error{ "Not supported format" };
			}
			break;
		}

	}

	void buffer::buffer_data(format the_format, const std::byte data[], size_t size_in_bytes, uint32_t frequency)
	{
		if (!m_handle) m_handle = gen_handle();
		if (m_handle) alBufferData(m_handle, format_to_AL_enum(the_format), data, static_cast<ALsizei>(size_in_bytes), frequency);
	}

	float buffer::get_duration() const
	{
		ALint size_in_bytes;
		ALint channels;
		ALint bits;
		ALint frequency;

		ALuint handle = get_handle();

		if (!handle) return 0;

		AL_CALL(alGetBufferi(handle, AL_SIZE, &size_in_bytes));
		AL_CALL(alGetBufferi(handle, AL_CHANNELS, &channels));
		AL_CALL(alGetBufferi(handle, AL_BITS, &bits));
		AL_CALL(alGetBufferi(handle, AL_FREQUENCY, &frequency));

		auto length_in_samples = size_in_bytes * 8 / (channels * bits);

		return static_cast<float>(length_in_samples) / static_cast<float>(frequency);
	}

	int32_t buffer::format_to_AL_enum(format the_format)
	{
		std::array<ALenum, 4> format_names{ AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8, AL_FORMAT_STEREO16 };
		return format_names[static_cast<std::underlying_type_t<decltype(the_format)>>(the_format)];
	}

	uint32_t buffer::gen_handle()
	{
		ALuint name = 0;
		AL_CALL(alGenBuffers(1, &name));

		return name;
	}

	void buffer::delete_handle(uint32_t handle)
	{
		ALuint name = handle;

		if (name)
			AL_CALL(alDeleteBuffers(1, &name));
	}
}
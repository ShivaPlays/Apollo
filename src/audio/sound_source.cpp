#include "audio/sound_source.h"

#include <AL/al.h>
#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif

#include "audio/sound_interface.h"
#include "audio/sound_buffer.h"

#include "utility/al_check.h"

#if not defined(AL_DIRECT_CHANNELS_SOFT)
#define AL_DIRECT_CHANNELS_SOFT						0x1033
#endif

#ifndef AL_SOURCE_SPATIALIZE_SOFT
	#define AL_SOURCE_SPATIALIZE_SOFT				0x1214
#endif

#ifndef AL_AUTO_SOFT
	#define AL_AUTO_SOFT							0x0002
#endif

namespace age
{
	sound_source::sound_source()
		: m_attached_sound{ nullptr }
		, m_attached_buffer{ AL_NONE }
	{}

	sound_source::sound_source(uint32_t handle)
		: m_attached_sound{ nullptr }
		, m_handle{ handle }
		, m_attached_buffer{ AL_NONE }
	{}

	sound_source::sound_source(constructor_key, uint32_t handle)
		: sound_source{ handle }
	{}

	void sound_source::play()
	{
		if (ensure_handle()) AL_CALL(alSourcePlay(m_handle));
	}

	void sound_source::stop()
	{
		if (ensure_handle()) AL_CALL(alSourceStop(m_handle));
	}

	void sound_source::pause()
	{
		if (ensure_handle()) AL_CALL(alSourcePause(m_handle));
	}

	void sound_source::rewind()
	{
		if (ensure_handle()) AL_CALL(alSourceRewind(m_handle));
	}

	void sound_source::set_position(const glm::vec3& value)
	{
		if (ensure_handle()) AL_CALL(alSource3f(m_handle, AL_POSITION, value.x, value.y, value.z));
	}

	glm::vec3 sound_source::get_position() const
	{
		ALfloat value[3] = { 0.0f, 0.0f, 0.0f };
		if (ensure_handle()) AL_CALL(alGetSource3f(m_handle, AL_POSITION, &value[0], &value[1], &value[2]));

		return glm::vec3{ value[0], value[1], value[2] };
	}

	void sound_source::set_pitch(float value)
	{
		if (ensure_handle()) AL_CALL(alSourcef(m_handle, AL_PITCH, value));
	}

	float sound_source::get_pitch() const
	{
		ALfloat value{};
		if (ensure_handle()) AL_CALL(alGetSourcef(m_handle, AL_PITCH, &value));

		return value;
	}

	void sound_source::set_volume(float value)
	{
		if (ensure_handle()) AL_CALL(alSourcef(m_handle, AL_GAIN, value));
	}

	float sound_source::get_volume() const
	{
		ALfloat value{};
		if (ensure_handle()) AL_CALL(alGetSourcef(m_handle, AL_GAIN, &value));

		return value;
	}

	void sound_source::set_reference_distance(float value)
	{
		if (ensure_handle()) AL_CALL(alSourcef(m_handle, AL_REFERENCE_DISTANCE, value));
	}

	float sound_source::get_reference_distance() const
	{
		ALfloat value{};
		if (ensure_handle()) AL_CALL(alGetSourcef(m_handle, AL_REFERENCE_DISTANCE, &value));

		return value;
	}

	void sound_source::set_attenuation(float value)
	{
		if (ensure_handle()) AL_CALL(alSourcef(m_handle, AL_ROLLOFF_FACTOR, value));
	}

	float sound_source::get_attenuation() const
	{
		ALfloat value{};
		if (ensure_handle()) AL_CALL(alGetSourcef(m_handle, AL_ROLLOFF_FACTOR, &value));

		return value;
	}

	void sound_source::set_relative_to_listener(bool value)
	{
		if (ensure_handle()) AL_CALL(alSourcei(m_handle, AL_SOURCE_RELATIVE, value ? 1 : 0));
	}

	bool sound_source::get_relative_to_listener() const
	{
		ALint value{};
		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_SOURCE_RELATIVE, &value));

		return value == AL_TRUE;
	}

	void sound_source::set_direct_channels(bool value)
	{
		if (ensure_handle()) AL_CALL(alSourcei(m_handle, AL_DIRECT_CHANNELS_SOFT, value ? 1 : 0));
	}

	bool sound_source::get_direct_channels() const
	{
		ALint value{};
		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_DIRECT_CHANNELS_SOFT, &value));

		return value == AL_TRUE;
	}

	void sound_source::set_looping(bool value)
	{
		if (ensure_handle()) AL_CALL(alSourcei(m_handle, AL_LOOPING, value ? 1 : 0));
	}

	bool sound_source::get_looping() const
	{
		ALint value{};
		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_LOOPING, &value));

		return value == AL_TRUE;
	}

	void sound_source::set_buffer(const sound_buffer& value)
	{
		auto new_buffer = value.get_handle();

		if (m_attached_buffer != new_buffer)
		{
			if (ensure_handle())
			{
				AL_CALL(alSourcei(m_handle, AL_BUFFER, new_buffer));
				m_attached_buffer = new_buffer;
			}
		}
	}

	bool sound_source::has_buffer_attached(const sound_buffer& value) const
	{
		auto b_handle = value.get_handle();

		return b_handle && (m_attached_buffer != AL_NONE) && (m_attached_buffer == b_handle);
	}

	void sound_source::detach_buffer(const sound_buffer& value)
	{
		if (has_buffer_attached(value))
		{
			if (auto state = get_state(); state == sound_state::playing || state == sound_state::paused)
				stop();

			if (ensure_handle())
			{
				AL_CALL(alSourcei(m_handle, AL_BUFFER, AL_NONE));
				m_attached_buffer = AL_NONE;
			}
		}
	}

	void sound_source::queue_buffer(sound_queue_buffer value)
	{
		ALuint handle = value.get_handle();

		if (ensure_handle()) AL_CALL(alSourceQueueBuffers(m_handle, 1, &handle));
	}

	uint32_t sound_source::get_num_queued_buffers() const
	{
		ALint result = 0;

		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_BUFFERS_QUEUED, &result));

		return static_cast<uint32_t>(result);
	}

	uint32_t sound_source::get_num_processed_buffers() const
	{
		ALint result = 0;

		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_BUFFERS_PROCESSED, &result));

		return static_cast<uint32_t>(result);
	}

	sound_queue_buffer sound_source::unqueue_buffer()
	{
		ALuint buffer = 0;

		if (ensure_handle()) AL_CALL(alSourceUnqueueBuffers(m_handle, 1, &buffer));

		return sound_queue_buffer{ buffer };
	}

	void sound_source::clear_buffers()
	{
		//stop if still playing
		if (auto state = get_state(); state == sound_state::playing || state == sound_state::paused)
			stop();

		//get rid of an eventual bound buffer
		if (ensure_handle()) AL_CALL(alSourcei(m_handle, AL_BUFFER, AL_NONE));
	}

	void sound_source::invalidate()
	{

	}

	sound_state sound_source::get_state() const
	{
		ALint state = AL_INITIAL;

		if (ensure_handle()) AL_CALL(alGetSourcei(m_handle, AL_SOURCE_STATE, &state));

		switch (state)
		{
		case AL_PLAYING:
			return sound_state::playing;
		case AL_PAUSED:
			return sound_state::paused;
		case AL_STOPPED:
		case AL_INITIAL:
			return sound_state::stopped;
		default:
			return sound_state::stopped;
		}
	}

	bool sound_source::ensure_handle() const
	{
		if (!m_handle) m_handle = gen_handle();

		return m_handle != 0;
	}

	void sound_source::enable_source_spatialize()
	{
		if (ensure_handle()) AL_CALL(alSourcei(m_handle, AL_SOURCE_SPATIALIZE_SOFT, AL_AUTO_SOFT));
	}


	uint32_t sound_source::gen_handle()
	{
		ALuint name = 0;
		AL_CALL(alGenSources(1, &name));

		return name;
	}

	void sound_source::delete_handle(uint32_t handle)
	{
		if (handle) AL_CALL(alDeleteSources(1, &handle));
	}
}
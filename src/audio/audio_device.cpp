#include "audio/audio_device.h"

#include <AL/al.h>
#include <AL/alc.h>

#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>

#include <stdexcept>
#include <sstream>
#include <cstring>

#include "audio/sound.h"

#include "utility/al_check.h"

namespace age
{
	audio_device::audio_device()
		: m_device{ nullptr }
		, m_context{ nullptr }
		, m_alcReopenDeviceSOFT_ptr { nullptr }
		, m_next_pool_index{ 0 }
		, m_is_initialised{ false }
		, m_is_direct_channles_available{ false }
		, m_source_spatialize_available{ false }
	{}

	audio_device::~audio_device()
	{
		destroy_context_and_close_device();
	}

	channel_guard audio_device::get_free_channel(bool reserved)
	{
		const auto pool_size = m_audio_channels.size();

		for (size_t i = 0; i < pool_size; ++i)
		{
			size_t idx = (m_next_pool_index + i) % pool_size;
			auto& channel = m_audio_channels[idx];

			if (channel.try_acquire())
			{
				auto guard = channel_guard{ &channel };
				if (channel.is_free() && !channel.is_reserved())
				{
					channel.set_reserved(reserved);
					m_next_pool_index = (idx + 1) % pool_size;
					return guard;
				}
			}
		}

		return channel_guard{ nullptr };
	}

	void audio_device::pause()
	{
		if (m_device) ALC_CALL(static_cast<ALCdevice*>(m_device), alcDevicePauseSOFT(static_cast<ALCdevice*>(m_device)));
	}

	void audio_device::resume()
	{
		if (m_device) ALC_CALL(static_cast<ALCdevice*>(m_device), alcDeviceResumeSOFT(static_cast<ALCdevice*>(m_device)));
	}

	bool audio_device::is_connected() const
	{
		if (m_device)
		{
			ALCint connected = ALC_TRUE;

			ALC_CALL(static_cast<ALCdevice*>(m_device), alcGetIntegerv(static_cast<ALCdevice*>(m_device), ALC_CONNECTED, 1, &connected));
			return (connected == ALC_TRUE);
		}

		return false;
	}

	bool audio_device::reopen()
	{
		if (m_device)
		{
			auto device = static_cast<ALCdevice*>(m_device);

			auto alcReopenDeviceSOFT = reinterpret_cast<LPALCREOPENDEVICESOFT>(m_alcReopenDeviceSOFT_ptr);
			return ALC_CALL(device, alcReopenDeviceSOFT(device, m_device_name.empty() ? nullptr : m_device_name.c_str(), nullptr)) == ALC_TRUE;
		}

		return false;
	}

	void audio_device::stop_all_sounds()
	{
		auto stop_source_sound = [](audio_channel& channel) -> void
		{
			if (auto sound = channel.get_owner())
				sound->stop();

			channel.get_source().stop();
		};

		for (auto& channel : m_audio_channels)
			stop_source_sound(channel);
	}

	void audio_device::remove_buffer_from_active_sources(const sound_buffer& buffer)
	{
		for (auto& channel: m_audio_channels)
			channel.get_source().detach_buffer(buffer);
	}

	bool audio_device::is_initialised() const
	{
		return m_is_initialised;
	}

	bool audio_device::is_direct_channels_available() const
	{
		return m_is_direct_channles_available;
	}

	audio_device& audio_device::get()
	{
		static audio_device audio_device_instance;
		return audio_device_instance;
	}

	std::vector<std::string_view> audio_device::get_device_names()
	{
		auto device_specifier_string = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
		auto result = std::vector<std::string_view>{};
		result.reserve(8);

		{
			size_t index = 0;
			do
			{
				result.emplace_back(&device_specifier_string[index]);
				index = strlen(&device_specifier_string[index]) + 1;

			} while (device_specifier_string[index] != '\0');
		}

		return result;
	}

	void audio_device::init()
	{
		std::lock_guard lock{ s_device_mutex };
		get().init(nullptr);
	}

	void audio_device::init(std::string_view device_name)
	{
		std::lock_guard lock{ s_device_mutex };
		get().init(device_name.data());
	}

	void audio_device::destroy()
	{
		std::lock_guard lock{ s_device_mutex };
		get().destroy_context_and_close_device();
	}

	void audio_device::set_listener_volume(float value)
	{
		m_listener_volume = value;
		AL_CALL(alListenerf(AL_GAIN, m_listener_volume));
	}

	float audio_device::get_listener_volume()
	{
		return m_listener_volume;
	}

	void audio_device::set_listener_position(const glm::vec3& value)
	{
		m_listener_position = value;
		AL_CALL(alListener3f(AL_POSITION, m_listener_position.x, m_listener_position.y, m_listener_position.z));
	}

	const glm::vec3& audio_device::get_listener_position()
	{
		return m_listener_position;
	}

	void audio_device::set_listener_direction(const glm::vec3& value)
	{
		m_listener_direction = value;

		float orientation[] = { m_listener_direction.x,
								m_listener_direction.y,
								m_listener_direction.z,
								m_listener_up_vector.x,
								m_listener_up_vector.y,
								m_listener_up_vector.z };

		AL_CALL(alListenerfv(AL_ORIENTATION, orientation));
	}

	const glm::vec3& audio_device::get_listener_direction()
	{
		return m_listener_direction;
	}

	void audio_device::set_listener_up_vector(const glm::vec3& value)
	{
		m_listener_up_vector = value;
		float orientation[] = { m_listener_direction.x,
								m_listener_direction.y,
								m_listener_direction.z,
								m_listener_up_vector.x,
								m_listener_up_vector.y,
								m_listener_up_vector.z };

		AL_CALL(alListenerfv(AL_ORIENTATION, orientation));
	}

	const glm::vec3& audio_device::get_listener_up_vector()
	{
		return m_listener_up_vector;
	}

	audio_channel* audio_device::play_buffer(const sound_buffer &buffer, const sound_properties &properties)
	{
		auto guard = get_free_channel(properties.looping);

		if (!guard) return nullptr;
		auto& source = guard->get_source();

		source.set_buffer(buffer);
		source.set_position(properties.position);
		source.set_relative_to_listener(properties.relative_to_listener);
		source.set_volume(properties.volume);
		source.set_pitch(properties.pitch);
		source.set_attenuation(properties.attenuation);
		source.set_reference_distance(properties.min_distance);
		source.set_looping(properties.looping);

		source.play();

		return &*guard;
	}

	void audio_device::init(const char* device_name)
	{
		destroy_context_and_close_device();
		open_device_and_create_context(device_name);
		setup_channels();

		m_device_name = device_name ? device_name : std::string{};
		m_is_initialised = true;
	}

	void audio_device::open_device_and_create_context(const char* device_name)
	{
		const ALCchar* default_device = device_name ? device_name : alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
		m_device = alcOpenDevice(default_device);

		if (!m_device)
		{
			ALCenum error = alcGetError(nullptr);
			std::stringstream ss;
			ss << "Failed to open audio device, with error: " << error;

			throw std::runtime_error{ ss.str() };
		}

		std::array<ALCint, 5> attributes = { ALC_MONO_SOURCES, MAX_SOURCES, ALC_STEREO_SOURCES, MAX_SOURCES, 0 };
		m_context = ALC_CALL(static_cast<ALCdevice*>(m_device), alcCreateContext(static_cast<ALCdevice*>(m_device), attributes.data()));

		if (!m_context)
		{
			std::stringstream ss;
			ss << "Failed to create the audio context";

			throw std::runtime_error{ ss.str() };
		}

		ALC_CALL(static_cast<ALCdevice*>(m_device), alcMakeContextCurrent(static_cast<ALCcontext*>(m_context)));

		// Apply the listener properties the user might have set
		float orientation[] = {m_listener_direction.x,
								m_listener_direction.y,
								m_listener_direction.z,
								m_listener_up_vector.x,
								m_listener_up_vector.y,
								m_listener_up_vector.z};
		AL_CALL(alListenerf(AL_GAIN, m_listener_volume));
		AL_CALL(alListener3f(AL_POSITION, m_listener_position.x, m_listener_position.y, m_listener_position.z));
		AL_CALL(alListenerfv(AL_ORIENTATION, orientation));

		if (alIsExtensionPresent("AL_SOFT_direct_channels")) m_is_direct_channles_available = true;
		if (alIsExtensionPresent("AL_SOFT_source_spatialize")) m_source_spatialize_available = true;

		if (ALC_CALL(static_cast<ALCdevice*>(m_device), alcIsExtensionPresent(static_cast<ALCdevice*>(m_device), "ALC_SOFT_reopen_device")))
		{
			m_alcReopenDeviceSOFT_ptr = alcGetProcAddress(static_cast<ALCdevice*>(m_device), "ALC_SOFT_reopen_device");
		}

		ALenum types[] = { AL_EVENT_TYPE_DISCONNECTED_SOFT };
		alEventControlSOFT(1, types, AL_TRUE);

		auto callback = [](ALenum  event_type, ALuint  object, ALuint  param, ALsizei length, const ALchar* message, void* user_ptr) noexcept
		{
			if (event_type == AL_EVENT_TYPE_DISCONNECTED_SOFT)
			{
				auto* self = static_cast<audio_device*>(user_ptr);
				try
				{
					self->m_maintenance_worker.add_job([self](){self->reopen();});
				}
				catch (...)
				{}
			}
		};

		alEventCallbackSOFT(callback, this);
	}

	void audio_device::destroy_context_and_close_device()
	{
		if (m_context)
		{
			{
				for (auto& channel : m_audio_channels)
					channel.detach_owner();

				m_audio_channels.clear();
			}

			alcMakeContextCurrent(nullptr);
			alcDestroyContext(static_cast<ALCcontext*>(m_context));
		}

		if (m_device)
		{
			alEventCallbackSOFT(nullptr, nullptr);
			alcCloseDevice(static_cast<ALCdevice*>(m_device));
		}

		m_context = nullptr;
		m_device = nullptr;

		m_alcReopenDeviceSOFT_ptr = nullptr;

		m_is_initialised = false;
		m_is_direct_channles_available = false;
		m_source_spatialize_available = false;

		m_device_name = {};
	}

	void audio_device::setup_channels()
	{
		std::array<ALuint, MAX_SOURCES> source_names{};

		m_audio_channels.clear();
		m_audio_channels.reserve(MAX_SOURCES);

		AL_CALL(alGenSources(MAX_SOURCES, source_names.data()));

		for (auto source_name : source_names)
		{
			m_audio_channels.emplace_back(sound_source::constructor_key{}, source_name);

			if (m_source_spatialize_available)
			{
				m_audio_channels.back().get_source().enable_source_spatialize();
			}
		}
	}
}
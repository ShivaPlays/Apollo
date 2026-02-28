#include "audio/audio_device.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <stdexcept>
#include <sstream>
#include <cstring>

#include "alext.h"
#include "audio/sound.h"

#include "utility/al_check.h"

namespace age
{
	audio_device::audio_device()
		: m_device{ nullptr }
		, m_context{ nullptr }
		, m_alcDevicePauseSOFT_ptr{ nullptr }
		, m_alcDeviceResumeSOFT_ptr{ nullptr }
		, m_alcReopenDeviceSOFT_ptr { nullptr }
		, m_is_initialised{ false }
		, m_is_direct_channles_available{ false }
		, m_source_spatialize_available{ false }
	{}

	audio_device::~audio_device()
	{
		destroy_context_and_close_device();
	}

	sound_source* audio_device::get_free_source(bool for_permanent_use) const
	{
		sound_source* result = nullptr;

		std::lock_guard container_lock{ m_source_queue_mutex };

		for (auto it = m_available_sources.begin(); it != m_available_sources.end(); ++it)
		{
			auto& sound_source = *it;

			if (sound_source->get_state() == sound_state::stopped)
			{
				sound_source->detach_sound();

				if (for_permanent_use)
					m_unavailable_sources.splice(m_unavailable_sources.end(), m_available_sources, it);
				else
					m_available_sources.splice(m_available_sources.end(), m_available_sources, it);

				result = sound_source;
				break;
			}
		}

		return result;
	}

	void audio_device::make_source_available(const sound_source* value) const
	{
		std::lock_guard container_lock{ m_source_queue_mutex };

		for (auto it = m_unavailable_sources.begin(); it != m_unavailable_sources.end(); ++it)
		{
			if (*it == value)
			{
				m_available_sources.splice(m_available_sources.end(), m_unavailable_sources, it);
				break;
			}
		}
	}

	void audio_device::pause()
	{
		if (m_device)
		{
			if (m_alcDevicePauseSOFT_ptr)
			{
				auto alcDevicePauseSOFT = reinterpret_cast<LPALCDEVICEPAUSESOFT>(m_alcDevicePauseSOFT_ptr);
				AL_CALL(alcDevicePauseSOFT(static_cast<ALCdevice*>(m_device)));
			}
			else
			{
				for (auto& source : m_sound_sources)
				{
					if (source.get_state() == sound_state::playing)
						source.pause();
				}
			}
		}
	}

	void audio_device::resume()
	{
		if (m_device)
		{
			if (m_alcDeviceResumeSOFT_ptr)
			{
				auto alcDeviceResumeSOFT = reinterpret_cast<LPALCDEVICEPAUSESOFT>(m_alcDeviceResumeSOFT_ptr);
				AL_CALL(alcDeviceResumeSOFT(static_cast<ALCdevice*>(m_device)));
			}
			else
			{
				for (auto& source : m_sound_sources)
					if (source.get_state() == sound_state::paused)
						source.play();
			}
		}
	}

	bool audio_device::is_connected() const
	{
		if (m_device)
		{
			ALCint connected = ALC_TRUE;

			alcGetIntegerv(static_cast<ALCdevice*>(m_device), ALC_CONNECTED, 1, &connected);
			return (connected == ALC_TRUE);
		}

		return false;
	}

	bool audio_device::reopen()
	{
		if (m_device && m_alcReopenDeviceSOFT_ptr)
		{
			auto alcReopenDeviceSOFT = reinterpret_cast<LPALCREOPENDEVICESOFT>(m_alcReopenDeviceSOFT_ptr);
			return AL_CALL(alcReopenDeviceSOFT(static_cast<ALCdevice*>(m_device), nullptr, nullptr)) == ALC_TRUE;
		}

		return false;
	}

	void audio_device::stop_all_sounds()
	{
		auto stop_source_sound = [](sound_source& source) -> void
		{
			if (auto sound = source.get_attached_sound())
				sound->stop();

			source.stop();
		};

		for (auto& source : m_sound_sources)
			stop_source_sound(source);
	}

	void audio_device::remove_buffer_from_active_sources(const sound_buffer& buffer)
	{
		for (auto& source: m_sound_sources)
			source.detach_buffer(buffer);
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
		auto device_specifier_string = AL_CALL(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
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

	sound_source * audio_device::play_buffer(const sound_buffer &buffer, const sound_properties &properties) const
	{
		auto source = get_free_source(properties.looping);

		if (nullptr == source) return nullptr;

		source->set_buffer(buffer);
		source->set_position(properties.position);
		source->set_relative_to_listener(properties.relative_to_listener);
		source->set_volume(properties.volume);
		source->set_pitch(properties.pitch);
		source->set_attenuation(properties.attenuation);
		source->set_min_distance(properties.min_distance);
		source->set_looping(properties.looping);

		source->play();

		return source;
	}

	void audio_device::init(const char* device_name)
	{
		destroy_context_and_close_device();
		open_device_and_create_context(device_name);
		setup_sources();

		m_device_name = device_name;
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
		m_context = alcCreateContext(static_cast<ALCdevice*>(m_device), attributes.data());

		if (!m_context)
		{
			std::stringstream ss;
			ss << "Failed to create the audio context";

			throw std::runtime_error{ ss.str() };
		}

		AL_CALL(alcMakeContextCurrent(static_cast<ALCcontext*>(m_context)));

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

		if (alcIsExtensionPresent(static_cast<ALCdevice*>(m_device), "ALC_SOFT_pause_device"))
		{
			// Fetch the function addresses
			m_alcDevicePauseSOFT_ptr = alcGetProcAddress(static_cast<ALCdevice*>(m_device), "alcDevicePauseSOFT");
			m_alcDeviceResumeSOFT_ptr = alcGetProcAddress(static_cast<ALCdevice*>(m_device), "alcDeviceResumeSOFT");
		}

		if (alcIsExtensionPresent(static_cast<ALCdevice*>(m_device), "ALC_SOFT_reopen_device"))
		{
			m_alcReopenDeviceSOFT_ptr = alcGetProcAddress(static_cast<ALCdevice*>(m_device), "ALC_SOFT_reopen_device");
		}
	}

	void audio_device::destroy_context_and_close_device()
	{
		if (m_context)
		{
			for (auto& source : m_sound_sources)
				source.detach_sound();

			m_sound_sources.clear();

			{
				std::lock_guard container_lock{ m_source_queue_mutex };

				m_available_sources.clear();
				m_unavailable_sources.clear();
			}

			alcMakeContextCurrent(nullptr);
			alcDestroyContext(static_cast<ALCcontext*>(m_context));
		}

		if (m_device)
			alcCloseDevice(static_cast<ALCdevice*>(m_device));

		m_context = nullptr;
		m_device = nullptr;

		m_alcDevicePauseSOFT_ptr = nullptr;
		m_alcDeviceResumeSOFT_ptr = nullptr;
		m_alcReopenDeviceSOFT_ptr = nullptr;

		m_is_initialised = false;
		m_is_direct_channles_available = false;
		m_source_spatialize_available = false;

		m_device_name = {};
	}

	void audio_device::setup_sources()
	{
		std::array<ALuint, MAX_SOURCES> source_names{};
		m_sound_sources.clear();
		m_sound_sources.reserve(MAX_SOURCES);
		AL_CALL(alGenSources(MAX_SOURCES, source_names.data()));

		for (auto source_name : source_names)
		{
			m_sound_sources.emplace_back(sound_source::constructor_key{}, source_name);

			if (m_source_spatialize_available)
			{
				m_sound_sources.back().enable_source_spatialize();
			}
		}

		std::lock_guard container_lock{ m_source_queue_mutex };
		m_available_sources.clear();
		for (auto& source : m_sound_sources)
			m_available_sources.emplace_back(&source);
	}
}
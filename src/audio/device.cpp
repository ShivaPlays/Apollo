#include "audio/device.h"

#include <AL/al.h>
#include <AL/alc.h>

#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>

#include <stdexcept>
#include <sstream>
#include <cstring>

#include "audio/config.h"
#include "audio/sound.h"

#include "audio/priv/al_check.h"


namespace age::audio
{
	device::device()
		: m_device{ nullptr }
		, m_context{ nullptr }
		, m_alcReopenDeviceSOFT_ptr { nullptr }
		, m_next_pool_index{ 0 }
		, m_is_initialised{ false }
	{}

	device::~device()
	{
		destroy_context_and_close_device();
	}

	channel_guard device::request_channel(bool reserved)
	{
		const auto pool_size = m_channels.size();

		size_t start_index = m_next_pool_index.load(std::memory_order_relaxed);

		for (size_t i = 0; i < pool_size; ++i)
		{
			size_t idx = (start_index + i) % pool_size;
			auto& channel = m_channels[idx];

			if (channel.try_acquire())
			{
				auto guard = channel_guard{ &channel };
				if (channel.is_free() && !channel.is_reserved())
				{
					channel.set_reserved(reserved);
					m_next_pool_index.store((idx + 1) % pool_size);
					return guard;
				}
			}
		}

		return channel_guard{ nullptr };
	}

	void device::pause()
	{
		if (m_device) ALC_CALL(static_cast<ALCdevice*>(m_device), alcDevicePauseSOFT(static_cast<ALCdevice*>(m_device)));
	}

	void device::resume()
	{
		if (m_device) ALC_CALL(static_cast<ALCdevice*>(m_device), alcDeviceResumeSOFT(static_cast<ALCdevice*>(m_device)));
	}

	bool device::is_connected() const
	{
		if (m_device)
		{
			ALCint connected = ALC_TRUE;

			ALC_CALL(static_cast<ALCdevice*>(m_device), alcGetIntegerv(static_cast<ALCdevice*>(m_device), ALC_CONNECTED, 1, &connected));
			return (connected == ALC_TRUE);
		}

		return false;
	}

	bool device::reopen()
	{
		if (m_device)
		{
			auto device = static_cast<ALCdevice*>(m_device);

			auto alcReopenDeviceSOFT = reinterpret_cast<LPALCREOPENDEVICESOFT>(m_alcReopenDeviceSOFT_ptr);
			return ALC_CALL(device, alcReopenDeviceSOFT(device, m_device_name.empty() ? nullptr : m_device_name.c_str(), nullptr)) == ALC_TRUE;
		}

		return false;
	}

	void device::stop_all_sounds()
	{
		auto stop_source_sound = [](channel& ch) -> void
		{
			auto lock = std::lock_guard{ ch.get_owner_mutex() };

			if (auto sound = ch.get_owner())
				sound->stop();

			ch.get_source().stop();
		};

		for (auto& channel : m_channels)
			stop_source_sound(channel);
	}

	void device::remove_buffer_from_active_sources(const buffer& buffer)
	{
		for (auto& channel: m_channels)
			channel.get_source().detach_buffer(buffer);
	}

	bool device::is_initialised() const
	{
		return m_is_initialised;
	}
	
	device& device::get()
	{
		static device audio_device_instance;
		return audio_device_instance;
	}

	std::vector<std::string_view> device::get_device_names()
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

	void device::init(uint8_t max_auxiliary_sends)
	{
		std::lock_guard lock{ s_device_mutex };
		get().init(nullptr, max_auxiliary_sends);
	}

	void device::init(std::string_view device_name, uint8_t max_auxiliary_sends)
	{
		std::lock_guard lock{ s_device_mutex };
		get().init(device_name.data(), max_auxiliary_sends);
	}

	void device::destroy()
	{
		std::lock_guard lock{ s_device_mutex };
		get().destroy_context_and_close_device();
	}

	void device::set_listener_volume(float value)
	{
		m_listener_volume = value;
		AL_CALL(alListenerf(AL_GAIN, m_listener_volume));
	}

	float device::get_listener_volume()
	{
		return m_listener_volume;
	}

	void device::set_listener_position(const glm::vec3& value)
	{
		m_listener_position = value;
		AL_CALL(alListener3f(AL_POSITION, m_listener_position.x, m_listener_position.y, m_listener_position.z));
	}

	const glm::vec3& device::get_listener_position()
	{
		return m_listener_position;
	}

	void device::set_listener_direction(const glm::vec3& value)
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

	const glm::vec3& device::get_listener_direction()
	{
		return m_listener_direction;
	}

	void device::set_listener_up_vector(const glm::vec3& value)
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

	const glm::vec3& device::get_listener_up_vector()
	{
		return m_listener_up_vector;
	}

	channel_guard device::play_buffer(const buffer &buffer, const properties &properties, uint8_t bus)
	{
		auto guard = request_channel(properties.looping);

		if (auto channel = guard.get())
		{
			channel->set_auxiliary_bus(bus);
			channel->attach_buffer(buffer);
			channel->apply_properties(properties);

			channel->play();
		}

		return guard;
	}

	void device::init(const char* device_name, uint8_t max_auxiliary_sends)
	{
		destroy_context_and_close_device();
		open_device_and_create_context(device_name, max_auxiliary_sends);
		setup_channels();

		m_device_name = device_name ? device_name : std::string{};
		m_is_initialised = true;
	}

	void device::open_device_and_create_context(const char* device_name, uint8_t max_auxiliary_sends)
	{
		const ALCchar* default_device = device_name ? device_name : alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
		m_device = alcOpenDevice(default_device);

		if (max_auxiliary_sends > config::MAX_AUXILIARY_SENDS) max_auxiliary_sends = config::MAX_AUXILIARY_SENDS;

		if (!m_device)
		{
			ALCenum error = alcGetError(nullptr);
			std::stringstream ss;
			ss << "Failed to open audio device, with error: " << error;

			throw std::runtime_error{ ss.str() };
		}

		std::array<ALCint, 7> attributes = { ALC_MONO_SOURCES, config::MAX_SOURCES, ALC_STEREO_SOURCES, config::MAX_SOURCES, ALC_MAX_AUXILIARY_SENDS, max_auxiliary_sends, 0 };
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
				auto* self = static_cast<device*>(user_ptr);
				try
				{
					self->m_maintenance_worker.add_job([self](){self->reopen();});
				}
				catch (...)
				{}
			}
		};

		alEventCallbackSOFT(callback, this);

		//For testing the ALC_MAX_AUXILIARY_SENDS number
		/*
		ALint actual_sends = 0;
		alcGetIntegerv(static_cast<ALCdevice*>(m_device), ALC_MAX_AUXILIARY_SENDS, 1, &actual_sends);
		std::cout << actual_sends;
		*/
	}

	void device::destroy_context_and_close_device()
	{
		if (m_context)
		{
			{
				for (auto& channel : m_channels)
					channel.detach_owner();

				m_channels.clear();
			}

			for (auto& g : m_auxiliary_buses) g.release();

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
		m_device_name = {};
	}

	void device::setup_channels()
	{
		std::array<ALuint, config::MAX_SOURCES> source_names{};

		m_channels.clear();
		m_channels.reserve(config::MAX_SOURCES);

		m_source_to_index.clear();
		m_source_to_index.reserve(config::MAX_SOURCES);

		AL_CALL(alGenSources(config::MAX_SOURCES, source_names.data()));

		for (auto source_name : source_names)
		{
			auto& ch = m_channels.emplace_back(source::constructor_key{}, source_name);

			m_source_to_index[source_name] = m_channels.size() - 1;
			ch.get_source().enable_source_spatialize();

		}
	}
}
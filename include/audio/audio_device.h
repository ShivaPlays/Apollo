#pragma once

#include <array>
#include <vector>
#include <list>
#include <string_view>
#include <mutex>
#include <atomic>
#include <glm/vec3.hpp>

#include "../system/background_worker.h"

#include "sound_properties.h"
#include "sound_source.h"
#include "audio_channel.h"
#include "channel_guard.h"

namespace age
{
	class sound;
	class sound_source;

	class audio_device
	{
	public:
		~audio_device();
	public:
		static audio_device& get();
		static std::vector<std::string_view> get_device_names();

		static void init();
		static void init(std::string_view device_name);
		static void destroy();

		static void set_listener_volume(float value);
		static float get_listener_volume();

		static void set_listener_position(const glm::vec3& value);
		static const glm::vec3& get_listener_position();
		
		static void set_listener_direction(const glm::vec3& value);
		static const glm::vec3& get_listener_direction();

		static void set_listener_up_vector(const glm::vec3& value);
		static const glm::vec3& get_listener_up_vector();

		audio_channel* play_buffer(const sound_buffer& buffer, const sound_properties& properties);
		channel_guard get_free_channel(bool reserved = false);

		void pause();
		void resume();

		bool is_connected() const;
		bool reopen();

		void stop_all_sounds();
		void remove_buffer_from_active_sources(const sound_buffer& buffer);

		bool is_initialised() const;
		bool is_direct_channels_available() const;
	protected:

	private:
		static inline std::mutex s_device_mutex;

		inline static constexpr uint32_t MAX_SOURCES = 256;

		inline static float m_listener_volume = 1.0f;
		inline static glm::vec3 m_listener_position{ 0.0f, 0.0f, 0.0f };
		inline static glm::vec3 m_listener_direction{ 0.0f, 0.0f, -1.0f };
		inline static glm::vec3 m_listener_up_vector{ 0.0f, 1.0f, 0.0f };

		audio_device();
		audio_device(const audio_device& other) = delete;
		audio_device(audio_device&& other) = delete;

		void init(const char* device_name);
		void open_device_and_create_context(const char* device_name);
		void destroy_context_and_close_device();

		void setup_channels();

		background_worker m_maintenance_worker;
		std::string m_device_name;

		void* m_device;
		void* m_context;

		void* m_alcReopenDeviceSOFT_ptr;

		std::atomic<size_t> m_next_pool_index;
		std::vector<audio_channel> m_audio_channels;

		bool m_is_initialised;
		bool m_is_direct_channles_available;
		bool m_source_spatialize_available;
	};
}
#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <list>
#include <string_view>
#include <mutex>
#include <atomic>
#include <glm/vec3.hpp>

#include "../core/background_worker.h"

#include "properties.h"
#include "source.h"
#include "channel.h"
#include "channel_guard.h"
#include "effect/auxiliary_send_group.h"
#include "effect/dedicated_dialog.h"

namespace age::audio
{
	class sound;
	class source;

	class device
	{
	public:
		~device();
	public:
		static device& get();
		static std::vector<std::string_view> get_device_names();

		static void init(uint8_t max_auxiliary_sends = 4);
		static void init(std::string_view device_name, uint8_t max_auxiliary_sends = 4);
		static void destroy();

		static void set_listener_volume(float value);
		static float get_listener_volume();

		static void set_listener_position(const glm::vec3& value);
		static const glm::vec3& get_listener_position();
		
		static void set_listener_direction(const glm::vec3& value);
		static const glm::vec3& get_listener_direction();

		static void set_listener_up_vector(const glm::vec3& value);
		static const glm::vec3& get_listener_up_vector();

		channel_guard play_buffer(const buffer& buffer, const properties& properties, uint8_t bus = 0);
		channel_guard get_free_channel(bool reserved = false);

		void pause();
		void resume();

		bool is_connected() const;
		bool reopen();

		void stop_all_sounds();
		void remove_buffer_from_active_sources(const buffer& buffer);

		bool is_initialised() const;
		bool is_direct_channels_available() const;

		const effect::auxiliary_send_group& get_auxiliary_send_group(uint8_t index) const { return m_auxiliary_buses[index]; }
		effect::auxiliary_send_group& get_auxiliary_send_group(uint8_t index) { return m_auxiliary_buses[index]; }

		const effect::effect_interface& get_bus_passthrough() const { return m_bus_passthrough; }
	protected:

	private:
		static inline std::mutex s_device_mutex;

		inline static float m_listener_volume = 1.0f;
		inline static glm::vec3 m_listener_position{ 0.0f, 0.0f, 0.0f };
		inline static glm::vec3 m_listener_direction{ 0.0f, 0.0f, -1.0f };
		inline static glm::vec3 m_listener_up_vector{ 0.0f, 1.0f, 0.0f };

		device();
		device(const device& other) = delete;
		device(device&& other) = delete;

		void init(const char* device_name, uint8_t max_auxiliary_sends);
		void open_device_and_create_context(const char* device_name, uint8_t max_auxiliary_sends);
		void destroy_context_and_close_device();

		void setup_channels();

		std::array<effect::auxiliary_send_group, 256> m_auxiliary_buses;

		core::background_worker m_maintenance_worker;

		effect::dedicated_dialog m_bus_passthrough;

		std::string m_device_name;

		void* m_device;
		void* m_context;

		void* m_alcReopenDeviceSOFT_ptr;

		std::atomic<size_t> m_next_pool_index;
		std::vector<channel> m_channels;
		std::unordered_map<uint32_t, size_t> m_source_to_index;

		bool m_is_initialised;
	};
}
#pragma once

#include "sound_interface.h"

#include <string_view>
#include <istream>
#include <vector>
#include <array>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <variant>
#include <cstddef>

#include "buffer.h"
#include "source.h"
#include "stream.h"
#include "../core/background_worker.h"

namespace age::audio
{
	class music
		: public sound_interface
	{
	public:
		struct command
		{
			using path_array = std::array<char, 1024>;

			enum class type{open, play, stop, buffer};

			type cmd_type = type::stop;
			std::variant<
				std::monostate,
				path_array,
				std::istream*,
				std::shared_ptr<std::istream>,
				std::pair<std::byte*, size_t>,
				size_t,
				bool
			> data = std::monostate{};
		};

		class command_buffer
		{
		public:
			command_buffer()
			{
				m_buffer.resize(BUFFER_SIZE);
			}
		public:
			template <typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, command>>>
			void add(T&& value)
			{
				size_t next_head = (m_head + 1) % m_buffer.size();

				if (next_head == m_tail)
				{
					// Emergency fallback: The queue is full! Time to resize.
					grow_buffer();
					next_head = (m_head + 1) % m_buffer.size();
				}

				m_buffer[m_head] = std::forward<T>(value);
				m_head = next_head;
			}

			command pop()
			{
				if (m_head == m_tail) return command{};

				size_t current_tail = m_tail;
				m_tail = (m_tail + 1) % m_buffer.size();

				return std::move(m_buffer[current_tail]);
			}

			bool empty() const
			{
				return m_head == m_tail;
			}

		protected:

		private:
			static constexpr size_t BUFFER_SIZE = 8;

			void grow_buffer()
			{
				size_t old_size = m_buffer.size();
				size_t new_size = old_size * 2; // Double the capacity

				std::vector<command> new_buffer{new_size};

				// Copy elements from m_tail to m_head into the beginning of the new vector
				size_t current = m_tail;
				size_t new_index = 0;

				while (current != m_head)
				{
					new_buffer[new_index++] = std::move(m_buffer[current]);
					current = (current + 1) % old_size;
				}

				// Reset indexes to match the fresh linear layout
				m_tail = 0;
				m_head = new_index;
				m_buffer = std::move(new_buffer);
			}

			std::vector<command> m_buffer;

			size_t m_head = 0;
			size_t m_tail = 0;
		};

		music();
		music(const music& other) = delete;
		music(music&& other) noexcept = delete;

		music& operator = (const music& other) = delete;
		music& operator = (music&& other) noexcept = delete;

		~music() override;
	public:
		void play() override;
		void stop() override;
		void pause() override;
		void on_channel_lost(channel& channel) override;
		void on_queued_buffers_processed(size_t num_buffers) override;

		void update_looping(bool value) override;

		void open(std::string_view fn);
		void open(std::istream& is);
		void open(std::shared_ptr<std::istream> is);
		void open(std::byte data[], size_t size);

		state get_state() const;

	protected:

	private:

		inline static constexpr size_t NUM_BUFFERS = 4;
		inline static constexpr size_t BUFFER_SAMPLES = 8192;

		template <typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, command>>>
		void add_command(T&& command)
		{
			std::lock_guard lock{ m_command_mutex };
			m_command_buffer.add(std::forward<T>(command));
			m_command_cv.notify_one();
		}

		void open_from_stream(std::istream& is);

		//void worker_loop_new();
		void worker_loop();

		mutable std::mutex m_command_mutex;

		std::condition_variable m_command_cv;

		std::array<buffer, NUM_BUFFERS> m_buffers;
		std::vector<std::byte> m_samples_buffer;

		command_buffer m_command_buffer;

		stream::info m_sound_stream_info;

		core::background_worker m_background_worker;
		std::shared_ptr<std::istream> m_active_istream;
		std::istream* m_requested_stream;

		std::unique_ptr<stream> m_sound_stream;
		std::atomic<state> m_internal_state;

		std::atomic<bool> m_looping{ false };

		bool m_alive;
	};
}
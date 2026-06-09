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
		struct music_command
		{
			enum class type{open, play, stop, pause};

			type cmd_type = type::stop;
			std::variant<
				std::array<char, 256>,
				std::istream*,
				std::unique_ptr<std::istream>,
				std::pair<std::byte*, size_t>,
				bool
			> data = std::array<char, 256>{0};
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
		void on_channel_lost() override;

		void update_looping(bool value) override;

		void open(std::string_view fn);
		void open(std::istream& is);
		void open(std::unique_ptr<std::istream> is);
		void open(std::byte data[], size_t size);

		state get_state() const;

	protected:

	private:
		inline static constexpr size_t NUM_BUFFERS = 4;
		inline static constexpr size_t BUFFER_SAMPLES = 8192;

		void open_from_stream(std::istream& is);

		void worker_loop();

		mutable std::mutex m_command_mutex;

		std::condition_variable m_command_cv;

		std::array<buffer, NUM_BUFFERS> m_buffers;
		std::vector<std::byte> m_samples_buffer;

		std::vector<music_command> m_command_queue;

		stream::info m_sound_stream_info;

		core::background_worker m_background_worker;
		std::unique_ptr<std::istream> m_active_istream;
		std::istream* m_requested_stream;

		std::unique_ptr<stream> m_sound_stream;
		std::atomic<state> m_internal_state;

		std::atomic<bool> m_looping{ false };

		bool m_alive;
	};
}
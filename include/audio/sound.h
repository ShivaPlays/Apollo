#pragma once
#include "sound_interface.h"

#include "state.h"
#include <glm/vec3.hpp>

namespace age::audio
{
	class source;
	class buffer;
	
	class sound
		: public sound_interface
	{
	public:
		friend class audio_device;
		friend class source;

		sound();
		sound(const sound& other) = default;
		sound(sound&& other) noexcept = default;

		sound& operator = (const sound& other) = default;
		sound& operator = (sound&& other) noexcept = default;

		virtual ~sound() override;

	public:
		void play(bool looped = false) override;
		void stop() override;
		void pause() override;

		state get_state() const;

		void set_buffer(const buffer* value);
		const buffer* get_buffer() const;

	protected:

	private:
		const buffer* m_buffer;
	};
}
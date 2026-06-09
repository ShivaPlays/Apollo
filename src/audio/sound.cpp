#include "audio/sound.h"

#include "audio/source.h"
#include "audio/device.h"

namespace age::audio
{
	sound::sound()
		: m_buffer{ nullptr }
	{}

	sound::~sound()
	{
		sound::stop();
	}

	void sound::play()
	{
		if (!m_buffer) return;

		bool done = false;

		get_channel_link().execute_on_channel([&done](channel& chan) {
			if (chan.get_state() == state::paused)
			{
				chan.play();
				done = true;
			}
		});

		if (done) return;

		auto guard = device::get().play_buffer(*m_buffer, get_properties());
		if (guard) attach_channel(guard);
	}

	void sound::stop()
	{
		get_channel_link().stop();
	}

	void sound::pause()
	{
		get_channel_link().pause();
	}

	state sound::get_state() const
	{
		return get_channel_link().get_state();
	}

	void sound::set_buffer(const buffer* value)
	{
		m_buffer = value;
	}

	const buffer* sound::get_buffer() const
	{
		return m_buffer;
	}
}
#include "audio/source.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/sound_interface.h"
#include "audio/buffer.h"

#include "audio/priv/al_check.h"
#include "audio/effect/slot.h"
#include "audio/effect/auxiliary_send_group.h"

namespace age::audio
{
	source::source()
		: m_attached_buffer{ AL_NONE }
	{}

	source::source(uint32_t handle)
		: m_handle{ handle }
		, m_attached_buffer{ AL_NONE }
	{
		apply_properties(properties{}, true);
	}

	source::source(constructor_key, uint32_t handle)
		: source{ handle }
	{}

	void source::play()
	{
		if (realize()) AL_CALL(alSourcePlay(m_handle));
	}

	void source::stop()
	{
		if (realize()) AL_CALL(alSourceStop(m_handle));
	}

	void source::pause()
	{
		if (realize()) AL_CALL(alSourcePause(m_handle));
	}

	void source::rewind()
	{
		if (realize()) AL_CALL(alSourceRewind(m_handle));
	}

	void source::apply_properties(const properties& properties, bool force)
	{
		if (realize())
		{
			alDeferUpdatesSOFT();

			stage_properties(properties, force);

			alProcessUpdatesSOFT();

			m_properties = properties;
		}
	}

	void source::set_position(const glm::vec3& value)
	{
		if (m_properties.position != value)
		{
			if (realize()) AL_CALL(alSource3f(m_handle, AL_POSITION, value.x, value.y, value.z));

			m_properties.position = value;
		}
	}

	const glm::vec3& source::get_position() const
	{
		return m_properties.position;
	}

	void source::set_velocity(const glm::vec3 &value)
	{
		if (m_properties.velocity != value)
		{
			if (realize()) AL_CALL(alSource3f(m_handle, AL_VELOCITY, value.x, value.y, value.z));

			m_properties.velocity = value;
		}
	}

	const glm::vec3& source::get_velocity() const
	{
		return m_properties.velocity;
	}

	void source::set_direction(const glm::vec3& value)
	{
		if (m_properties.direction != value)
		{
			if (realize()) AL_CALL(alSource3f(m_handle, AL_DIRECTION, value.x, value.y, value.z));

			m_properties.direction = value;
		}
	}

	const glm::vec3& source::get_direction() const
	{
		return m_properties.direction;
	}

	void source::set_radius(float value)
	{
		if (m_properties.source_radius != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_SOURCE_RADIUS, value));

			m_properties.source_radius = value;
		}
	}

	float source::get_radius() const
	{
		return m_properties.source_radius;
	}

	void source::set_cone_inner_angle(float value)
	{
		if (m_properties.cone_inner_angle != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_CONE_INNER_ANGLE, value));

			m_properties.cone_inner_angle = value;
		}
	}

	float source::get_cone_inner_angle() const
	{
		return m_properties.cone_inner_angle;
	}

	void source::set_cone_outer_angle(float value)
	{
		if (m_properties.cone_outer_angle != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_CONE_OUTER_ANGLE, value));

			m_properties.cone_outer_angle = value;
		}
	}

	float source::get_cone_outer_angle() const
	{
		return m_properties.cone_outer_angle;
	}

	void source::set_cone_outer_gain(float value)
	{
		if (m_properties.cone_outer_gain != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_CONE_OUTER_GAIN, value));

			m_properties.cone_outer_gain = value;
		}
	}

	float source::get_cone_outer_gain() const
	{
		return m_properties.cone_outer_gain;
	}

	void source::set_pitch(float value)
	{
		if (m_properties.pitch != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_PITCH, value));

			m_properties.pitch = value;
		}
	}

	float source::get_pitch() const
	{
		return m_properties.pitch;
	}

	void source::set_volume(float value)
	{
		if (m_properties.volume != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_GAIN, value));

			m_properties.volume = value;
		}
	}

	float source::get_volume() const
	{
		return m_properties.volume;
	}

	void source::set_min_gain(float value)
	{
		if (m_properties.min_gain != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_MIN_GAIN, value));

			m_properties.min_gain = value;
		}
	}

	float source::get_min_gain() const
	{
		return m_properties.min_gain;
	}

	void source::set_max_gain(float value)
	{
		if (m_properties.max_gain != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_MAX_GAIN, value));

			m_properties.max_gain = value;
		}
	}

	float source::get_max_gain() const
	{
		return m_properties.max_gain;
	}

	void source::set_max_distance(float value)
	{
		if (m_properties.max_distance != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_MAX_DISTANCE, value));

			m_properties.max_distance = value;
		}
	}

	float source::get_max_distance() const
	{
		return m_properties.max_distance;
	}

	void source::set_rolloff_factor(float value)
	{
		if (m_properties.rolloff_factor != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_ROLLOFF_FACTOR, value));

			m_properties.rolloff_factor = value;
		}
	}

	float source::get_rolloff_factor() const
	{
		return m_properties.rolloff_factor;
	}

	void source::set_reference_distance(float value)
	{
		if (m_properties.reference_distance != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_REFERENCE_DISTANCE, value));

			m_properties.reference_distance = value;
		}
	}

	float source::get_reference_distance() const
	{
		return m_properties.reference_distance;
	}

	void source::set_relative_to_listener(bool value)
	{
		if (m_properties.relative_to_listener != value)
		{
			if (realize()) AL_CALL(alSourcei(m_handle, AL_SOURCE_RELATIVE, value ? 1 : 0));

			m_properties.relative_to_listener = value;
		}
	}

	bool source::get_relative_to_listener() const
	{
		return m_properties.relative_to_listener;
	}

	void source::set_air_absorption_factor(float value)
	{
		if (m_properties.air_absorption_factor != value)
		{
			if (realize()) AL_CALL(alSourcef(m_handle, AL_AIR_ABSORPTION_FACTOR, value));

			m_properties.air_absorption_factor = value;
		}
	}

	float source::get_air_absorption_factor() const
	{
		return m_properties.air_absorption_factor;
	}

	void source::set_direct_channels(bool value)
	{
		if (m_properties.direct_channels != value)
		{
			if (realize()) AL_CALL(alSourcei(m_handle, AL_DIRECT_CHANNELS_SOFT, value ? 1 : 0));

			m_properties.direct_channels = value;
		}
	}

	bool source::get_direct_channels() const
	{
		return m_properties.direct_channels;
	}

	void source::set_looping(bool value)
	{
		if (m_properties.looping != value)
		{
			if (realize()) AL_CALL(alSourcei(m_handle, AL_LOOPING, value ? 1 : 0));

			m_properties.looping = value;
		}
	}

	bool source::get_looping() const
	{
		return m_properties.looping;
	}

	void source::attach_buffer(const buffer& value)
	{
		auto new_buffer = value.get_handle();

		if (m_attached_buffer != new_buffer)
		{
			if (realize())
			{
				AL_CALL(alSourcei(m_handle, AL_BUFFER, new_buffer));

				m_attached_buffer = new_buffer;
			}
		}
	}

	bool source::has_buffer_attached(const buffer& value) const
	{
		if (const auto b_handle = value.get_handle())
		{
			for (auto b : m_queued_buffers)
				if (b == b_handle) return true;

			return (m_attached_buffer != AL_NONE) && (m_attached_buffer == b_handle);
		}

		return false;
	}

	void source::detach_buffer(const buffer& value)
	{
		if (has_buffer_attached(value))
		{
			if (auto state = get_state(); state == state::playing || state == state::paused)
				stop();

			if (realize())
			{
				AL_CALL(alSourcei(m_handle, AL_BUFFER, AL_NONE));
				m_attached_buffer = AL_NONE;
				m_queued_buffers.clear();
			}
		}
	}

	void source::enqueue_buffer(queue_buffer value)
	{
		ALuint handle = value.get_handle();

		if (realize())
		{
			AL_CALL(alSourceQueueBuffers(m_handle, 1, &handle));
			m_queued_buffers.push_back(handle);
		}
	}

	uint32_t source::get_num_queued_buffers() const
	{
		ALint result = 0;

		if (m_handle) AL_CALL(alGetSourcei(m_handle, AL_BUFFERS_QUEUED, &result));

		return static_cast<uint32_t>(result);
	}

	uint32_t source::get_num_processed_buffers() const
	{
		ALint result = 0;

		if (m_handle) AL_CALL(alGetSourcei(m_handle, AL_BUFFERS_PROCESSED, &result));

		return static_cast<uint32_t>(result);
	}

	queue_buffer source::unqueue_buffer()
	{
		ALuint buffer = 0;

		if (realize())
		{
			AL_CALL(alSourceUnqueueBuffers(m_handle, 1, &buffer));
			if (!m_queued_buffers.empty() && buffer) m_queued_buffers.erase(m_queued_buffers.begin());
		}

		return queue_buffer{ buffer };
	}

	void source::clear_buffers()
	{
		//stop if still playing
		if (auto state = get_state(); state == state::playing || state == state::paused)
			stop();

		//get rid of an eventual bound buffer
		if (realize()) AL_CALL(alSourcei(m_handle, AL_BUFFER, AL_NONE));
	}


	void source::set_effect_slot(size_t index, const effect::slot &slot)
	{
		if (index < m_slots.size() && m_slots[index] != slot.get_handle())
		{
			if (realize())
			{
				auto slot_id = slot.get_handle();

				AL_CALL(alSource3i(m_handle, AL_AUXILIARY_SEND_FILTER, slot_id, index, AL_FILTER_NULL));

				m_slots[index] = slot_id;
			}
		}
	}

	void source::update_effect_slots(const effect::auxiliary_send_group &group)
	{
		const auto& slots = group.get_slots();
		for (size_t i = 0; i < slots.size(); ++i) set_effect_slot(i, slots[i]);
	}

	void source::reset_effect_slots()
	{
		for (size_t i = 0; i < m_slots.size(); ++i)
		{
			if (auto& sf = m_slots[i]; sf != AL_EFFECTSLOT_NULL)
			{
				if (realize()) AL_CALL(alSource3i(m_handle, AL_AUXILIARY_SEND_FILTER, sf, i, AL_FILTER_NULL));
				sf = AL_EFFECTSLOT_NULL;
			}
		}
	}

	void source::invalidate()
	{
		m_handle = 0;
	}

	state source::get_state() const
	{
		ALint state = AL_INITIAL;

		if (m_handle) AL_CALL(alGetSourcei(m_handle, AL_SOURCE_STATE, &state));

		switch (state)
		{
		case AL_PLAYING:
			return state::playing;
		case AL_PAUSED:
			return state::paused;
		//case AL_STOPPED:
		//case AL_INITIAL:
		//	return sound_state::stopped;
		default:
			return state::stopped;
		}
	}

	void source::stage_properties(const properties& properties, bool force)
	{
		if (force || m_properties.position != properties.position)
			AL_CALL(alSource3f(m_handle, AL_POSITION, properties.position.x, properties.position.y, properties.position.z));
		if (force || m_properties.velocity != properties.velocity)
			AL_CALL(alSource3f(m_handle, AL_VELOCITY, properties.velocity.x, properties.velocity.y, properties.velocity.z));
		if (force || m_properties.direction != properties.direction)
			AL_CALL(alSource3f(m_handle, AL_DIRECTION, properties.direction.x, properties.direction.y, properties.direction.z));
		if (force || m_properties.source_radius != properties.source_radius)
			AL_CALL(alSourcef(m_handle, AL_SOURCE_RADIUS, properties.source_radius));
		if (force || m_properties.cone_inner_angle != properties.cone_inner_angle)
			AL_CALL(alSourcef(m_handle, AL_CONE_INNER_ANGLE, properties.cone_inner_angle));
		if (force || m_properties.cone_outer_angle != properties.cone_outer_angle)
			AL_CALL(alSourcef(m_handle, AL_CONE_OUTER_ANGLE, properties.cone_outer_angle));
		if (force || m_properties.cone_outer_gain != properties.cone_outer_gain)
			AL_CALL(alSourcef(m_handle, AL_CONE_OUTER_GAIN, properties.cone_outer_gain));
		if (force || m_properties.pitch != properties.pitch)
			AL_CALL(alSourcef(m_handle, AL_PITCH, properties.pitch));
		if (force || m_properties.volume != properties.volume)
			AL_CALL(alSourcef(m_handle, AL_GAIN, properties.volume));
		if (force || m_properties.min_gain != properties.min_gain)
			AL_CALL(alSourcef(m_handle, AL_MIN_GAIN, properties.min_gain));
		if (force || m_properties.max_gain != properties.max_gain)
			AL_CALL(alSourcef(m_handle, AL_MAX_GAIN, properties.max_gain));
		if (force || m_properties.max_distance != properties.max_distance)
			AL_CALL(alSourcef(m_handle, AL_MAX_DISTANCE, properties.max_distance));
		if (force || m_properties.rolloff_factor != properties.rolloff_factor)
			AL_CALL(alSourcef(m_handle, AL_ROLLOFF_FACTOR, properties.rolloff_factor));
		if (force || m_properties.reference_distance != properties.reference_distance)
			AL_CALL(alSourcef(m_handle, AL_REFERENCE_DISTANCE, properties.reference_distance));
		if (force || m_properties.air_absorption_factor != properties.air_absorption_factor)
			AL_CALL(alSourcef(m_handle, AL_AIR_ABSORPTION_FACTOR, properties.air_absorption_factor));
		if (force || m_properties.relative_to_listener != properties.relative_to_listener)
			AL_CALL(alSourcei(m_handle, AL_SOURCE_RELATIVE, properties.relative_to_listener ? 1 : 0));
		if (force || m_properties.direct_channels != properties.direct_channels)
			AL_CALL(alSourcei(m_handle, AL_DIRECT_CHANNELS_SOFT, properties.direct_channels ? 1 : 0));
		if (force || m_properties.looping != properties.looping)
			AL_CALL(alSourcei(m_handle, AL_LOOPING, properties.looping ? 1 : 0));
	}

	bool source::realize()
	{
		if (!m_handle)
		{
			m_handle = gen_handle();
			apply_properties(properties{}, true);
			reset_effect_slots();

			m_attached_buffer = AL_NONE;
		}

		return m_handle != 0;
	}

	void source::enable_source_spatialize()
	{
		if (realize()) AL_CALL(alSourcei(m_handle, AL_SOURCE_SPATIALIZE_SOFT, AL_AUTO_SOFT));
	}

	uint32_t source::gen_handle()
	{
		ALuint name = 0;
		AL_CALL(alGenSources(1, &name));

		return name;
	}

	void source::delete_handle(uint32_t handle)
	{
		AL_CALL(alDeleteSources(1, &handle));
	}
}
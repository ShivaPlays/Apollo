//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/frequency_shifter.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    inline ALint direction_to_int(frequency_shifter::direction value)
    {
        constexpr std::array<int32_t, 3> al_direction{ AL_FREQUENCY_SHIFTER_DIRECTION_DOWN,  AL_FREQUENCY_SHIFTER_DIRECTION_UP, AL_FREQUENCY_SHIFTER_DIRECTION_OFF };

        if (value < frequency_shifter::direction::count) return al_direction[static_cast<size_t>(value)];

        return AL_FREQUENCY_SHIFTER_DIRECTION_DOWN;
    }

    inline frequency_shifter::direction int_to_direction(ALint value)
    {
        switch (value)
        {
            case AL_FREQUENCY_SHIFTER_DIRECTION_DOWN:   return frequency_shifter::direction::down;
            case AL_FREQUENCY_SHIFTER_DIRECTION_UP:     return frequency_shifter::direction::up;
            case AL_FREQUENCY_SHIFTER_DIRECTION_OFF:    return frequency_shifter::direction::off;
            default:                                    return frequency_shifter::direction::down;
        }
    }

    void frequency_shifter::set_frequency(float value)
    {
        if (m_frequency != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_FREQUENCY_SHIFTER_FREQUENCY, value));
            m_frequency = value;
        }
    }

    float frequency_shifter::get_frequency() const
    {
        return m_frequency;
    }

    void frequency_shifter::set_left_direction(direction value)
    {
        ALint left_direction = direction_to_int(value);

        if (m_left_direction != left_direction)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, left_direction));
            m_left_direction = left_direction;
        }
    }

    frequency_shifter::direction frequency_shifter::get_left_direction() const
    {
        return int_to_direction(m_left_direction);
    }

    void frequency_shifter::set_right_direction(direction value)
    {
        ALint right_direction = direction_to_int(value);

        if (m_right_direction != right_direction)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, right_direction));
            m_right_direction = right_direction;
        }
    }

    frequency_shifter::direction frequency_shifter::get_right_direction() const
    {
        return int_to_direction(m_right_direction);
    }

    void frequency_shifter::init() const
    {
        auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_FREQUENCY_SHIFTER));

        AL_CALL(alEffectf(handle, AL_FREQUENCY_SHIFTER_FREQUENCY, m_frequency));

        AL_CALL(alEffectf(handle, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, m_left_direction));
        AL_CALL(alEffectf(handle, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, m_right_direction));
    }
}
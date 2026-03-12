//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "effect_interface.h"

namespace age::audio::effect
{
    class reverb : public effect_interface
    {
    public:

    public:
        void set_air_absorption_gain_hf(float value);
        float get_air_absorption_gain_hf() const;

        void set_density(float value);
        float get_density() const;

        void set_diffusion(float value);
        float get_diffusion() const;

        void set_gain(float value);
        float get_gain() const;

        void set_gain_hf(float value);
        float get_gain_hf() const;

        void set_decay_time(float value);
        float get_decay_time() const;

        void set_decay_hf_ratio(float value);
        float get_decay_hf_ratio() const;

        void set_reflections_gain(float value);
        float get_reflections_gain() const;

        void set_reflections_delay(float value);
        float get_reflections_delay() const;

        void set_late_reverb_gain(float value);
        float get_late_reverb_gain() const;

        void set_late_reverb_delay(float value);
        float get_late_reverb_delay() const;

        void set_room_rolloff_factor(float value);
        float get_room_rolloff_factor() const;

        void set_decay_hfl_limit(bool value);
        bool get_decay_hfl_limit() const;

    protected:
        void init() override;

    private:
        float m_air_absorption_gain_hf{ 0.994 };

        float m_density{ 1.0f };
        float m_diffusion{ 1.0f };

        float m_gain{ 0.32f };
        float m_gain_hf{ 0.89f };

        float m_decay_time{ 1.49f };
        float m_decay_hf_ratio{ 0.83f };

        float m_reflections_gain{ 0.05f };
        float m_reflections_delay{ 0.007f };

        float m_late_reverb_gain{ 1.26f };
        float m_late_reverb_delay{ 0.011f };

        float m_room_rolloff_factor{ 0.0f };

        bool m_decay_hfl_limit{ true };

    };
}
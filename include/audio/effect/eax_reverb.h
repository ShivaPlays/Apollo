//
// Created by skaldi on 21.03.26.
//

#pragma once

#include "effect_interface.h"

#include <glm/vec3.hpp>

namespace age::audio::effect
{
    class eax_reverb : public effect_interface
    {
    public:

    public:
        void set_density(float value);
        float get_density() const;

        void set_diffusion(float value);
        float get_diffusion() const;

        void set_gain(float value);
        float get_gain() const;

        void set_gain_hf(float value);
        float get_gain_hf() const;

        void set_gain_lf(float value);
        float get_gain_lf() const;

        void set_decay_time(float value);
        float get_decay_time() const;

        void set_decay_hf_ratio(float value);
        float get_decay_hf_ratio() const;

        void set_decay_lf_ratio(float value);
        float get_decay_lf_ratio() const;

        void set_reflections_gain(float value);
        float get_reflections_gain() const;

        void set_reflections_delay(float value);
        float get_reflections_delay() const;

        void set_reflections_pan(const glm::vec3& value);
        const glm::vec3& get_reflections_pan() const;

        void set_late_reverb_gain(float value);
        float get_late_reverb_gain() const;

        void set_late_reverb_delay(float value);
        float get_late_reverb_delay() const;

        void set_late_reverb_pan(const glm::vec3& value);
        const glm::vec3& get_late_reverb_pan() const;

        void set_echo_time(float value);
        float get_echo_time() const;

        void set_echo_depth(float value);
        float get_echo_depth() const;

        void set_modulation_time(float value);
        float get_modulation_time() const;

        void set_modulation_depth(float value);
        float get_modulation_depth() const;

        void set_air_absorption_gain_hf(float value);
        float get_air_absorption_gain_hf() const;

        void set_hf_reference(float value);
        float get_hf_reference() const;

        void set_lf_reference(float value);
        float get_lf_reference() const;

        void set_room_rolloff_factor(float value);
        float get_room_rolloff_factor() const;

        void set_decay_hf_limit(bool value);
        bool get_decay_hf_limit() const;

    protected:
        void init() const override;

    private:
        float m_density{ 1.0f };
        float m_diffusion{ 1.0f };
        float m_gain{ 0.32f };
        float m_gain_hf { 0.89f };
        float m_gain_lf { 1.0f };
        float m_decay_time{ 1.49f };
        float m_decay_hf_ratio{ 0.83f };
        float m_decay_lf_ratio{ 1.0f };
        float m_reflections_gain{ 0.05f };
        float m_reflections_delay{ 0.007f };
        glm::vec3 m_reflections_pan{ 0.0f };
        float m_late_reverb_gain{ 1.26f };
        float m_late_reverb_delay { 0.011f };
        glm::vec3 m_late_reverb_pan{ 0.0f };
        float m_echo_time{ 0.25f };
        float m_echo_depth{ 0.0f };
        float m_modulation_time { 0.25f };
        float m_modulation_depth{ 0.0f };
        float m_air_absorption_gain_hf{ 0.994f };
        float m_hf_reference{ 5000.0f };
        float m_lf_reference{ 250.0f };
        float m_room_rolloff_factor{ 0.0f };
        bool m_decay_hf_limit{ true };
    };
}
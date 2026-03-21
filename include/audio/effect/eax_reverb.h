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
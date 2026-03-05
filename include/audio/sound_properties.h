#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <limits>

namespace age
{
    struct sound_properties
    {
        glm::vec3 position{0.0f};
        glm::vec3 velocity{0.0f};
        glm::vec3 direction{0.0f};
        float source_radius = 0.0f;
        float cone_inner_angle = 360.0f;
        float cone_outer_angle = 360.0f;
        float cone_outer_gain = 0.0f;
        float pitch = 1.0f;
        float volume = 1.0f;
        float min_gain = 0.0f;
        float max_gain = 1.0f;
        float max_distance = std::numeric_limits<float>::max();
        float rolloff_factor = 1.0f;
        float reference_distance = 1.0f;
        float air_absorption_factor = 0.0f;
        bool relative_to_listener = false;
        bool direct_channels = false;
        bool looping = false;
    };
}
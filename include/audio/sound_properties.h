#pragma once

#include <glm/vec3.hpp>

namespace age
{
    struct sound_properties
    {
        glm::vec3 position{0.0f};
        float pitch = 1.0f;
        float volume = 1.0f;
        float min_distance = 1.0f;
        float attenuation = 1.0f;
        bool relative_to_listener = true;
        bool direct_channels = false;
        bool looping = false;
    };
}
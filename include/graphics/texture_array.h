//
// Created by skaldi on 02.07.26.
//

#pragma once

#include <cstdint>

#include "texture_interface.h"

#include "texture.h"
#include "image.h"

namespace age
{
    class texture_array : public texture_interface
    {
    public:
        texture_array();

        texture_array(const texture_array& other);
        texture_array(texture_array&& other) noexcept = default;

        texture_array& operator = (const texture_array& other);
        texture_array& operator = (texture_array&& other) noexcept = default;

        ~texture_array() override = default;

    public:
        void create(glm::uvec2 size, size_t layers);
        void create(glm::uvec2 size, size_t layers, uint8_t* data_ptr);
        void create(texture textures[], size_t num_textures);
        void create(image images[], size_t num_images);

        static uint32_t get_maximum_size();
        static uint32_t get_maximum_layers();
    protected:

    private:
        static void create_internal(glm::uvec2 size, size_t layers, uint8_t* data_ptr);

    };
}

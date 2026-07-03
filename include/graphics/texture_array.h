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

        void create(texture textures[], size_t num_textures);
        void create(image images[], size_t num_images);

    public:

    protected:

    private:

    };
}

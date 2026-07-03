//
// Created by skaldi on 02.07.26.
//

#include "graphics/texture_array.h"

#include "utility/gl_loader.h"

namespace age
{
    texture_array::texture_array()
        : texture_interface{ GL_TEXTURE_2D_ARRAY }
    {}

    void texture_array::create(texture textures[], size_t num_textures)
    {
        if (num_textures == 0) return;

        if (num_textures > 1)
        {
            auto tex_size = textures[0].get_size();

            for (size_t i = 1; i < num_textures; ++i)
            {
                if (tex_size != textures[i].get_size()) throw std::runtime_error("texture size mismatch");
            }
        }

        if (realize())
        {
            for (size_t i = 0; i < num_textures; ++i)
            {
                auto& text = textures[i];

                auto image = text.copy_to_image();
            }
        }
    }

    void texture_array::create(age::image images[], size_t num_images)
    {
        //Sanity check first 

        if (realize())
        {

        }
    }
}

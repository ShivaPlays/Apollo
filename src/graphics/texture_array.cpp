//
// Created by skaldi on 02.07.26.
//

#include "graphics/texture_array.h"

#include "utility/utility.h"
#include "utility/gl_check.h"
#include "utility/gl_loader.h"

namespace age
{
    //Helper function for my unique_handle
    inline void delete_framebuffer(GLuint handle) noexcept
    {
        GL_CALL(glDeleteFramebuffers(1, &handle));
    }

    texture_array::texture_array()
        : texture_interface{ GL_TEXTURE_2D_ARRAY }
    {}

    texture_array::texture_array(const texture_array& other)
        : texture_interface{ GL_TEXTURE_2D_ARRAY }
    {
        if (realize() && bind())
        {
            create_internal(glm::uvec2{other.get_size().x, other.get_size().y}, other.get_size().z, nullptr);

            GLuint fbo_name = 0;
            GL_CALL(glGenFramebuffers(1, &fbo_name));
            unique_handle<GLuint, delete_framebuffer> fbo{ fbo_name };

            GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
            std::unique_ptr<int, void(*)(int*)> unbind_guard(reinterpret_cast<int*>(1), [](int*) { GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0)); });

            //Loop through every layer and copy it over
            for (size_t layer = 0; layer < other.get_size().z; ++layer)
            {
                // Attach layer 'layer' of the SOURCE texture array to the Read Framebuffer
                // We use glFramebufferTextureLayer instead of glFramebufferTexture2D for arrays!
                GL_CALL(glFramebufferTextureLayer(
                    GL_READ_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    other.get_id(),
                    0,      // Mipmap level
                    layer        // The specific source layer index
                ));

                // Copy from the bound FBO layer directly into destination layer 'layer'
                GL_CALL(glCopyTexSubImage3D(
                    GL_TEXTURE_2D_ARRAY,
                    0,                      // Mipmap level
                    0, 0,            // Destination x, y offsets
                    layer,                 // Destination zoffset (the layer index!)
                    0, 0,                    // Source x, y coordinates from FBO
                    other.get_size().x, other.get_size().y
                ));
            }
        }
    }

    texture_array& texture_array::operator=(const texture_array& other)
    {
        if (this == &other) return *this;

        texture_array temp{ other };
        *this = std::move(temp);

        return *this;
    }

    void texture_array::create(glm::uvec2 size, size_t layers)
    {
        create(size, layers, nullptr);
    }

    void texture_array::create(glm::uvec2 size, size_t layers, uint8_t* data_ptr)
    {
        auto tex_size = glm::vec3{size, layers};
        set_size(glm::uvec3{tex_size});

        if (realize() && bind() && size.x > 0 && size.y > 0 && layers)
        {
            create_internal(size, layers, data_ptr);
        }
    }

    void texture_array::create(texture textures[], size_t num_textures)
    {
        if (num_textures == 0) return;

        auto tex_size = textures[0].get_size();

        if (num_textures > 1)
        {
            for (size_t i = 1; i < num_textures; ++i)
            {
                if (tex_size != textures[i].get_size()) throw std::runtime_error("texture size mismatch");
            }
        }

        tex_size.z = num_textures;
        set_size(glm::uvec3{tex_size});

        if (realize() && bind())
        {
            create_internal(tex_size, num_textures, nullptr);

            for (size_t i = 0; i < num_textures; ++i)
            {
                auto& tex = textures[i];

                GLuint fbo_name = 0;
                GL_CALL(glGenFramebuffers(1, &fbo_name));
                unique_handle<GLuint, delete_framebuffer> fbo{ fbo_name };

                GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
                std::unique_ptr<int, void(*)(int*)> unbind_guard(reinterpret_cast<int*>(1), [](int*) { GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); });

                //Attach your existing 2D source texture to the Read Framebuffer
                GL_CALL(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.get_id(), 0));

                // 3. Copy the screen/FBO pixels directly into the specified layer of your array
                // Note: 'layer' is passed as the zoffset (5th parameter)
                GL_CALL(glCopyTexSubImage3D(
                    GL_TEXTURE_2D_ARRAY,
                    0,              // Mipmap level
                    0, 0,    // xoffset, yoffset (destination offsets)
                    i,             // zoffset (This is the target layer index!)
                    0, 0,            // x, y (source coordinates from the FBO)
                    tex_size.x, tex_size.y
                ));
            }
        }
    }

    void texture_array::create(image images[], size_t num_images)
    {
        //Sanity check first
        if (num_images == 0) return;

        auto tex_size= images[0].get_size();

        if (num_images > 1)
        {
            for (size_t i = 1; i < num_images; ++i)
            {
                auto& img = images[i];

                if (tex_size != img.get_size()) throw std::runtime_error("texture size mismatch");
            }
        }

        if (realize() && bind())
        {
            std::vector<uint8_t> img_data{};
            img_data.reserve(tex_size.x * tex_size.y * num_images * 4);

            for (size_t i = 0; i < num_images; ++i)
            {
                auto pixel_ptr = images[i].get_pixel_ptr();

                img_data.insert(img_data.end(), pixel_ptr, pixel_ptr + tex_size.x * tex_size.y * 4);
            }

            create_internal(tex_size, num_images, img_data.data());
        }
    }

    void texture_array::create_internal(glm::uvec2 size, size_t layers, uint8_t* data_ptr)
    {
        GL_CALL(glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,                                   // Mipmap level
            GL_RGBA8,                                 // Internal format
            size.x, size.y, layers,
            0,                                  // Border
            GL_RGBA, GL_UNSIGNED_BYTE, data_ptr       // No initial data pointer
        ));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }
}

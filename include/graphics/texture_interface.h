//
// Created by skaldi on 02.07.26.
//

#pragma once

#include <cstdint>
#include <glm/vec3.hpp>

#include "../utility/utility.h"

namespace age
{
    class texture_interface
    {
    public:
        texture_interface(uint32_t type)
            : m_type(type)
        {}

        // Explicitly restore the move operations
        texture_interface(texture_interface&&) noexcept = default;
        texture_interface& operator=(texture_interface&&) noexcept = default;

        virtual ~texture_interface() = default;

    public:
        bool bind() const;
        static bool bind(const texture_interface* tex);

        glm::uvec3 get_size() const { return m_size;}

        uint32_t get_id() const { return m_handle; }

        void invalidate();

        static void reset_cache();

    protected:
        void set_size(glm::uvec3 value) { m_size = value; }

        uint32_t get_handle() const { return m_handle; }
        void set_handle(uint32_t hanlde) { m_handle = hanlde; }

        bool realize()
        {
            if (!m_handle) m_handle = gen_handle();

            return m_handle != 0;
        }

    private:
        static uint32_t gen_handle();
        static void delete_handle(uint32_t handle);

        glm::uvec3 m_size{1, 1, 1 };

        uint32_t m_type{};
        unique_handle<uint32_t, delete_handle> m_handle;
    };
}

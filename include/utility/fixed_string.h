//
// Created by skaldi on 08.02.26.
//

#pragma once

#include <array>
#include <string_view>
#include <cstring>
#include <algorithm>

namespace age
{
    template<size_t N>
    class fixed_string
    {
    public:
        fixed_string() = default;

        fixed_string(std::string_view sv)
        {
            size_t len = std::min(sv.length(), N - 1);
            std::memcpy(m_data.data(), sv.data(), len);
            m_data[len] = '\0';
            m_length = len;
        }

        const char* data() const { return m_data.data(); }
        size_t size() const { return m_length; }
        std::string_view view() const { return { m_data.data(), m_length }; }

        bool operator==(const fixed_string& other) const
        {
            return view() == other.view();
        }

        bool operator==(std::string_view sv) const
        {
            return view() == sv;
        }

    private:
        std::array<char, N> m_data{};
        size_t m_length = 0;
    };

    /*
    // The Transparent Hasher
    // For C++20 then
    struct fixed_string_hash
    {
        using is_transparent = void; // The magic switch

        size_t operator()(std::string_view sv) const
        {
            return std::hash<std::string_view>{}(sv);
        }

        template<size_t N>
        size_t operator()(const fixed_string<N>& fs) const
        {
            return std::hash<std::string_view>{}(fs.view());
        }
    };
    */
}
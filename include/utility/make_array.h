//
// Created by skaldi on 08.03.26.
//

#pragma once

#include <array>
#include <utility>
#include <cstddef>

namespace age::utility
{
    // This helper does the heavy lifting
    template <typename T, typename... Args, std::size_t... Is>
    std::array<T, sizeof...(Is)> make_array_impl(std::index_sequence<Is...>, Args&&... args)
    {
        // This expands: T{args...}, T{args...}, T{args...}
        return { ((static_cast<void>(Is), T{std::forward<Args>(args)...}))... };
    }

    // Your public API
    template <typename T, std::size_t N, typename... Args>
    std::array<T, N> make_array(Args&&... args)
    {
        return make_array_impl<T>(std::make_index_sequence<N>{}, std::forward<Args>(args)...);
    }
}
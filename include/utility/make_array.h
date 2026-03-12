#pragma once

#include <array>
#include <utility>
#include <cstddef>

namespace age::utility
{
    // The factory takes a function/lambda that receives the index
    template <typename T, std::size_t N, typename Factory>
    constexpr std::array<T, N> make_array_factory(Factory&& factory)
    {
        return []<std::size_t... Is>(Factory&& f, std::index_sequence<Is...>)
        {
            // Each element is constructed by calling the factory with the current index
            return std::array<T, N>{ f(Is)... };
        }(std::forward<Factory>(factory), std::make_index_sequence<N>{});
    }
}
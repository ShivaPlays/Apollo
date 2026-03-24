#pragma once

#include <array>
#include <utility>
#include <cstddef>

namespace age::utility
{
    namespace priv {
        // Separate the expansion into a dedicated helper
        template <typename T, std::size_t N, typename Factory, std::size_t... Is>
        constexpr std::array<T, N> make_array_helper(Factory&& factory, std::index_sequence<Is...>) {
            return { { std::forward<Factory>(factory)(Is)... } };
        }
    }

    template <typename T, std::size_t N, typename Factory>
    constexpr std::array<T, N> make_array_factory(Factory&& factory) {
        return priv::make_array_helper<T, N>(
            std::forward<Factory>(factory),
            std::make_index_sequence<N>{}
        );
    }
}


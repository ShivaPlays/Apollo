#pragma once

#include <stdint.h>
#include <cstring>

namespace age
{
	namespace endian
	{
		constexpr inline bool is_big_endian()
		{
#ifdef __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return true;
#else
			return false;
#endif
#else
			// Fallback for compilers that don't define the macro
			return false;
#endif
		}

		inline int32_t convert_to_int(const std::byte buffer[], std::size_t size)
		{
			int32_t a = 0;
			if (!is_big_endian())
				std::memcpy(&a, buffer, size);
			else
				for (std::size_t i = 0; i < size; ++i)
					reinterpret_cast<int8_t*>(&a)[3 - i] = std::to_integer<int8_t>(buffer[i]);

			return a;
		}
	}
}
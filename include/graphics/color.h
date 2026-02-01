#pragma once

#include <cstdint>

namespace age
{
	struct color
	{
		constexpr color()
		{}

		constexpr color(uint8_t p_red, uint8_t p_green, uint8_t p_blue, uint8_t p_alpha = 255)
			: r{ p_red }
			, g{ p_green }
			, b{ p_blue }
			, a{ p_alpha }
		{}

		uint8_t r{ 0 };
		uint8_t g{ 0 };
		uint8_t b{ 0 };
		uint8_t a{ 255 };

		static const color black;
		static const color white;
		static const color red;
		static const color green;
		static const color blue;
		static const color yellow;
		static const color magenta;
		static const color cyan;
		static const color transparent;
	};

	inline constexpr color color::black(0, 0, 0);
	inline constexpr color color::white(255, 255, 255);
	inline constexpr color color::red(255, 0, 0);
	inline constexpr color color::green(0, 255, 0);
	inline constexpr color color::blue(0, 0, 255);
	inline constexpr color color::yellow(255, 255, 0);
	inline constexpr color color::magenta(255, 0, 255);
	inline constexpr color color::cyan(0, 255, 255);
	inline constexpr color color::transparent(0, 0, 0, 0);

	inline bool operator == (const color& lhs, const color& rhs)
	{
		const uint32_t* lh = reinterpret_cast<const uint32_t*>(&lhs);
		const uint32_t* rh = reinterpret_cast<const uint32_t*>(&rhs);

		return *lh == *rh;
	}

	inline bool operator != (const color& lhs, const color& rhs)
	{
		return !(lhs == rhs);
	}
}
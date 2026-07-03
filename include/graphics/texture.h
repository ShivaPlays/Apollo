#pragma once

#include "texture_interface.h"

#include <string_view>
#include <istream>

#include <glm/vec3.hpp>
#include "rect.h"
#include "image.h"
#include "render_window.h"
#include "../utility/utility.h"

namespace age
{
	class texture : public texture_interface
	{
	public:
		friend class render_target;

		texture();

		texture(const texture& other);
		texture(texture&& other) noexcept = default;

		texture& operator = (const texture& other);
		texture& operator = (texture&& other) noexcept = default;

		~texture() override = default;

	public:
		void create(const glm::u32vec2& size);

		void load(std::string_view filename, const int_rect& area = int_rect{});
		void load(const std::byte data[], std::size_t size, const int_rect& area = int_rect{});
		void load(std::istream& is, const int_rect& area = int_rect{});
		void load(const image& img, const int_rect& area = int_rect{});

		void update(const uint8_t* pixels);
		void update(const uint8_t* pixels, const uint_rect& area);
		void update(const texture& other_texture);
		void update(const texture& other_texture, const glm::u32vec2& dest);
		void update(const image& img);
		void update(const image& img, const glm::u32vec2& dest);
		void update(const render_window& window);
		void update(const render_window& window, const glm::u32vec2& dest);

		image copy_to_image() const;

		void set_smooth(bool value);
		bool get_smooth() const;

		void set_srgb(bool value);
		bool get_srgb() const;

		void set_repeat(bool value);
		bool get_repeat() const;

		void generate_mipmap();
		void invalidate_mipmap();

		static uint32_t get_maximum_size();
	protected:

	private:

		bool m_smooth = false;
		bool m_srgb = false;
		bool m_repeat = false;
		bool m_has_mipmap = false;
	};
}
#pragma once

#include <string_view>
#include <istream>

#include <glm/vec2.hpp>
#include "rect.h"
#include "image.h"
#include "render_window.h"
#include "../utility/utility.h"

namespace age
{
	class texture
	{
	public:
		friend class render_target;

		texture() = default;

		texture(const texture& other);
		texture(texture&& other) = default;

		texture& operator = (const texture& other);
		texture& operator = (texture&& other) = default;

		~texture() = default;

	public:
		bool bind() const;
		
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

		const glm::uvec2& get_size() const;
		image copy_to_image() const;

		void set_smooth(bool value);
		bool get_smooth() const;

		void set_srgb(bool value);
		bool get_srgb() const;

		void set_repeat(bool value);
		bool get_repeat() const;

		void generate_mipmap();
		void invalidate_mipmap();

		uint32_t get_id() const;

		void invalidate();

		static bool bind(const texture* tex);
		static uint32_t get_maximum_size();

		static void reset_cache();

	protected:

	private:
		static uint32_t gen_handle();
		static void delete_handle(uint32_t handle);

		uint32_t get_handle() const { return m_handle; }

		glm::uvec2 m_size{1, 1}; //We use 1, 1 as standard size so that math calculations work

		unique_handle<uint32_t, delete_handle> m_handle;
		bool m_smooth = false;
		bool m_srgb = false;
		bool m_repeat = false;
		bool m_has_mipmap = false;
	};
}
#pragma once

#include <string>
#include <string_view>
#include <istream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <tuple>

#include "rect.h"
#include "texture.h"

namespace age
{
	class font
	{
	public:
		font();
		font(const font& copy) = delete;
		font(font&& other) noexcept;

		font& operator = (const font& other) = delete;
		font& operator = (font&& other) noexcept;

		~font();

	public:
		struct info
		{
			std::string family;
		};

		struct glyph
		{
			float advance = 0.0f;
			int32_t lsb_delta = 0;
			int32_t rsb_delta = 0;
			float_rect bounds;
			int_rect texture_rect;
		};

		void load(std::string_view fn);
		void load(const std::byte data[], size_t size_in_bytes);
		void load(std::unique_ptr<std::istream> in_stream);
		void load(std::istream& in_stream);

		const info& get_info() const;
		const glyph& get_glyph(uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness = 0.0f) const;
		bool has_glyph(uint32_t code_point) const;

		float get_kerning(uint32_t first, uint32_t second, uint32_t character_size, bool bold = false) const;
		float get_line_spacing(uint32_t character_size) const;

		float get_underline_position(uint32_t character_size) const;
		float get_underline_thickness(uint32_t character_size) const;

		const texture& get_texture(uint32_t character_size) const;

		void set_smooth(bool value);
		bool get_smooth() const;

		void pre_cache_glyphs(const std::u32string_view& letters, uint32_t character_size, bool bold, float outline_thickness);

	protected:

	private:
		struct row
		{
			row(unsigned int row_top, unsigned int row_height) 
				: width{ 0 }
				, height{ row_height }
				, top{ row_top }

			{}

			uint32_t width{};
			uint32_t height{};
			uint32_t top{};
		};

		struct page
		{
			explicit page(bool smooth);

			std::unordered_map<uint64_t, glyph> glyphs;
			texture page_texture;
			uint32_t next_row;
			std::vector<row> rows;
		};

		class font_handles;

		void load_from_stream(std::istream& in_stream);

		page& load_page(uint32_t character_size) const;
		glyph load_glyph(uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const;

		int_rect find_glyph_rect(page& page, const glm::u32vec2& size) const;

		void set_current_size(uint32_t character_size) const;

		void cleanup();

		info m_info;

		mutable std::unordered_map<uint32_t, page> m_pages;
		mutable std::vector<uint8_t> m_pixel_buffer;

		std::unique_ptr<font_handles> m_font_handles;
		std::unique_ptr<std::istream> m_stream;

		bool m_smooth = true;
	};
}
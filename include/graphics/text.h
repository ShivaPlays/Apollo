#pragma once

#include "transformable_2d.h"
#include "drawable.h"

#include <string>
#include <type_traits>

#include "font.h"
#include "color.h"
#include "rect.h"
#include "vertex_2d.h"

namespace age
{
	namespace text_styles
	{
		enum style
		{
			regular = 0,
			bold = 1 << 0,
			italic = 1 << 1,
			underlined = 1 << 2,
			strike_through = 1 << 3
		};
	}

	class text
		: public drawable
		, public transformable_2d
	{
	public:
		text();
		text(std::string_view the_text, const font& the_font, uint32_t the_character_size = 30);

		text(const text& other);
		text(text&& other) noexcept;

		text& operator = (const text& other);
		text& operator = (text&& other) noexcept;

	public:
		void set_string(std::string_view value);
		const std::string& get_string() const;

		void set_font(const font& the_font);
		const font* get_font() const;

		void set_character_size(uint32_t value);
		uint32_t get_character_size() const;

		void set_line_spacing(float factor);
		float get_line_spacing() const;

		void set_letter_spacing(float factor);
		float get_letter_spacing() const;

		void set_style(uint32_t flags);
		uint32_t get_style() const;

		void set_fill_color(const color& value);
		const color& get_fill_color() const;

		void set_outline_color(const color& value);
		const color& get_outline_color() const;

		void set_outline_thickness(float value);
		float get_outline_thickness() const;

		glm::vec2 find_character_pos(size_t index) const;

		float_rect get_local_bounds() const;

		float_rect get_global_bounds() const;

	protected:

	private:
		void draw(render_target& target, const render_states& states) const override;

		void ensure_geometry_is_updated() const;

		std::string m_string;
		const font* m_font;
		uint32_t m_character_size;
		float m_letter_spacing_factor;
		float m_line_spacing_factor;
		uint32_t m_style;
		color m_fill_color;
		color m_outline_color;
		float m_outline_thickness;

		mutable glm::uvec3 m_last_texture_size;
		mutable uint32_t m_last_texture_id;

		mutable std::vector<vertex_2d> m_vertices;
		mutable std::vector<vertex_2d> m_outline_vertices;
		mutable float_rect m_bounds;
		mutable bool m_geometry_needs_update;
	};
}
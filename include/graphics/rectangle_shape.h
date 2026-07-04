#pragma once

#include "shape_2d.h"

#include <array>

#include "vertex_2d.h"
#include "rect.h"

namespace age
{
	class texture_interface;

	class rectangle_shape
		: public shape_2d
	{
	public:
		rectangle_shape(const glm::vec2& size = glm::vec2{ 0.0f, 0.0f });

	public:

		void set_size(const glm::vec2& value);
		const glm::vec2& get_size() const;

		void set_texture(const texture_interface& value, bool reset_texture_rect = false);
		void set_texture_rect(const uint_rect& value);
		
		const uint_rect& get_texture_rect() const;

		void set_outline_thickness(float value);
		float get_outline_thickness() const;

		void set_fill_color(const color& value);
		const color& get_fill_color() const;

		void set_outline_color(const color& value);
		const color& get_outline_color() const;

		inline const std::array<vertex_2d, 4>& get_vertices() const { return m_vertices; }
		inline const std::array<uint32_t, 6>& get_indices() const { return m_indices; }

		inline const std::array<vertex_2d, 8>& get_outline_vertices() const { return m_outline_vertices; }
		inline const std::array<uint32_t, 24>& get_outline_indices() const { return m_outline_indices; }

	protected:

	private:
		virtual void draw(render_target& target, const render_states& states) const override;
		virtual vertex_data on_get_vertex_data() const override;

		void update_vertices();
		void update_outline();
		void update_tex_coords();

		std::array<vertex_2d, 4> m_vertices;
		std::array<uint32_t, 6> m_indices;

		std::array<vertex_2d, 8> m_outline_vertices;
		std::array<uint32_t, 24> m_outline_indices;

		uint_rect m_texture_rect;
		glm::vec2 m_size;

		const texture_interface* m_texture;

		float m_outline_thickness;
	};
}
#include "graphics/rectangle_shape.h"

#include "graphics/render_states.h"
#include "graphics/render_target.h"
#include "engine.h"

namespace age
{
	rectangle_shape::rectangle_shape(const glm::vec2& size)
		: m_vertices{	vertex_2d{ glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f} },
						vertex_2d{ glm::vec2{size.x, 0.0f}, glm::vec2{1.0f, 0.0f} },
						vertex_2d{ glm::vec2{size.x, size.y}, glm::vec2{1.0f, 1.0f} },
						vertex_2d{ glm::vec2{0.0f, size.y}, glm::vec2{0.0f, 1.0f} } }
		, m_indices{ 0, 1, 2, 2, 3, 0 }
		, m_outline_indices{ 0, 1, 2, 2, 3, 0, 1, 4, 5, 5, 2, 1, 4, 6, 7, 7, 5, 4, 6, 0, 3, 3, 7, 6 }
		, m_size{ size }
		, m_texture{ &engine::get_instance()->get_default_texture() }
		, m_outline_thickness{ 0.0f }
	{
		/*
		* Those are the outline vertices
		0                 1
		+-----------------+
		|    3       2    |
		|    +-------+    |
		|    |       |    |
		|    |       |    |
		|    |       |    |
		|    +-------+    |
		|    7       5    |
		+-----------------+
		6                 4
		*/
	}

	void rectangle_shape::set_size(const glm::vec2& value)
	{
		m_size = value;

		update_vertices();
		update_outline();
	}

	const glm::vec2& rectangle_shape::get_size() const
	{
		return m_size;
	}

	void rectangle_shape::set_texture(const texture_interface& value, bool reset_texture_rect)
	{
		bool update_coords = false;
		if (m_texture != &value)
			update_coords = true;

		m_texture = &value;

		if (reset_texture_rect || m_texture_rect == uint_rect{})
		{
			m_texture_rect = uint_rect{ glm::u32vec2{0, 0}, value.get_size() };
			update_coords = true;
		}

		if(update_coords)
			update_tex_coords();
	}

	void rectangle_shape::set_texture_rect(const uint_rect& value)
	{
		if (m_texture_rect != value)
		{
			m_texture_rect = value;
			update_tex_coords();
		}
	}

	const uint_rect& rectangle_shape::get_texture_rect() const
	{
		return m_texture_rect;
	}

	void rectangle_shape::set_texture_layer(int16_t value)
	{
		for (auto& vert : m_vertices) vert.tex_coords.z = static_cast<float>(value);
	}

	int16_t rectangle_shape::get_texture_layer() const
	{
		return static_cast<int16_t>(m_vertices[0].tex_coords.z);
	}

	void rectangle_shape::set_outline_thickness(float value)
	{
		m_outline_thickness = value;

		update_vertices();
		update_outline();
	}

	float rectangle_shape::get_outline_thickness() const
	{
		return m_outline_thickness;
	}

	void rectangle_shape::set_fill_color(const color& value)
	{
		for (auto& v : m_vertices) v.vertex_color = value;
	}

	const color& rectangle_shape::get_fill_color() const
	{
		return m_vertices[0].vertex_color;
	}

	void rectangle_shape::set_outline_color(const color& value)
	{
		for (auto& v : m_outline_vertices) v.vertex_color = value;
	}

	const color& rectangle_shape::get_outline_color() const
	{
		return m_outline_vertices[0].vertex_color;
	}

	void rectangle_shape::draw(render_target& target, const render_states& states) const
	{
		render_states states_copy = states;

		states_copy.get_transform() *= get_transform();
		//I just assume that m_texture is initialised properly. So all other constructors shall only be called after engine has been constructed
		states_copy.set_texture(*m_texture);

		target.draw(m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size(), states_copy);

		if (m_outline_thickness != 0.0f)
		{
			states_copy.set_texture(engine::get_instance()->get_default_texture());
			target.draw(m_outline_vertices.data(), m_outline_vertices.size(), m_outline_indices.data(), m_outline_indices.size(), states_copy);
		}
	}

	shape_2d::vertex_data rectangle_shape::on_get_vertex_data() const
	{
		return shape_2d::vertex_data{ m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size() };
	}

	void rectangle_shape::update_vertices()
	{
		glm::vec2 size_without_outline{ m_size.x - 2.0f * m_outline_thickness, m_size.y - 2.0f * m_outline_thickness };

		m_vertices[0].position = glm::vec2{ m_outline_thickness, m_outline_thickness };
		m_vertices[1].position = glm::vec2{ size_without_outline.x + m_outline_thickness, m_outline_thickness };
		m_vertices[2].position = glm::vec2{ size_without_outline.x + m_outline_thickness, size_without_outline.y + m_outline_thickness };
		m_vertices[3].position = glm::vec2{ m_outline_thickness, size_without_outline.y + m_outline_thickness };
	}

	void rectangle_shape::update_outline()
	{
		if (m_outline_thickness != 0.0f)
		{
			m_outline_vertices[3].position = m_vertices[0].position;
			m_outline_vertices[2].position = m_vertices[1].position;
			m_outline_vertices[5].position = m_vertices[2].position;
			m_outline_vertices[7].position = m_vertices[3].position;

			m_outline_vertices[0].position.x = m_vertices[0].position.x - m_outline_thickness;
			m_outline_vertices[0].position.y = m_vertices[0].position.y - m_outline_thickness;

			m_outline_vertices[1].position.x = m_vertices[1].position.x + m_outline_thickness;
			m_outline_vertices[1].position.y = m_vertices[1].position.y - m_outline_thickness;

			m_outline_vertices[4].position.x = m_vertices[2].position.x + m_outline_thickness;
			m_outline_vertices[4].position.y = m_vertices[2].position.y + m_outline_thickness;

			m_outline_vertices[6].position.x = m_vertices[3].position.x - m_outline_thickness;
			m_outline_vertices[6].position.y = m_vertices[3].position.y + m_outline_thickness;
		}
	}

	void rectangle_shape::update_tex_coords()
	{
		m_vertices[0].tex_coords = glm::vec3{ static_cast<float>(m_texture_rect.left), static_cast<float>(m_texture_rect.top), 0.0f };
		m_vertices[1].tex_coords = glm::vec3{ static_cast<float>(m_texture_rect.left + m_texture_rect.width), static_cast<float>(m_texture_rect.top), 0.0f };
		m_vertices[2].tex_coords = glm::vec3{ static_cast<float>(m_texture_rect.left + m_texture_rect.width), static_cast<float>(m_texture_rect.top + m_texture_rect.height), 0.0f };
		m_vertices[3].tex_coords = glm::vec3{ static_cast<float>(m_texture_rect.left), static_cast<float>(m_texture_rect.top + m_texture_rect.height), 0.0f };
	}
}
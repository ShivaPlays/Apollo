#include "graphics/circle_shape.h"

#include <stdexcept>

#include "graphics/render_states.h"

static constexpr float PI = 3.141592654f;

namespace age
{
	circle_shape::circle_shape(float radius, uint32_t resolution)
		: m_texture{ &engine::get_instance()->get_default_texture() }
		, m_resolution{ resolution }
		, m_radius{ radius }
		, m_outline_thickness{ 0.0f }
		, m_fill_color{ color::white }
	{
		if (resolution < 3)
			throw std::runtime_error{ "Failure creating circle: The resolution must be 3 or higher" };

		radius = glm::abs(radius);

		gen_vertices();
	}

	void circle_shape::set_resolution(uint32_t value)
	{
		if (m_resolution == value) return;

		if (value < 3)
			throw std::runtime_error{ "Failure creating circle: The resolution must be 3 or higher" };

		m_resolution = value;

		gen_vertices();

		if (m_outline_thickness)
		{
			scale_for_outline();
			gen_outline_vertices();
		}

		update_fill_color();
		update_outline_color();
	}

	uint32_t circle_shape::get_resolution() const
	{
		return m_resolution;
	}

	void circle_shape::set_radius(float value)
	{
		if (m_radius == value) return;

		m_radius = value;
	}

	float circle_shape::get_radius() const
	{
		return m_radius;
	}

	void circle_shape::set_outline_thickness(float value)
	{
		if (m_outline_thickness == value) return;

		if (value >= m_radius)
			throw std::runtime_error{ "Failure setting outline_thickness: The outline thickness must be smaller than the radius" };

		m_outline_thickness = value;

		scale_for_outline();
		if (m_outline_thickness)
		{
			gen_outline_vertices();
		}
	}

	float circle_shape::get_outline_thickness() const
	{
		return m_outline_thickness;
	}

	void circle_shape::set_fill_color(const color& value)
	{
		if (m_fill_color == value) return;

		m_fill_color = value;
		update_fill_color();
	}

	const color& circle_shape::get_fill_color() const
	{
		return m_fill_color;
	}

	void circle_shape::set_outline_color(const color& value)
	{
		if (m_outline_color == value) return;

		m_outline_color = value;
		update_outline_color();
	}

	const color& circle_shape::get_outline_color() const
	{
		return m_outline_color;
	}

	void circle_shape::draw(render_target& target, const render_states& states) const
	{
		render_states states_copy = states;

		states_copy.get_transform() *= get_transform();
		states_copy.set_texture(*m_texture);

		target.draw(m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size(), states_copy);

		if (m_outline_thickness != 0.0f)
		{
			states_copy.set_texture(engine::get_instance()->get_default_texture());
			target.draw(m_outline_vertices.data(), m_outline_vertices.size(), m_outline_indices.data(), m_outline_indices.size(), states_copy);
		}
	}

	shape_2d::vertex_data circle_shape::on_get_vertex_data() const
	{
		return shape_2d::vertex_data{ m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size() };
	}

	void circle_shape::gen_vertices()
	{
		m_vertices.clear();
		m_indices.clear();

		m_vertices.reserve(m_resolution + 1);
		m_indices.reserve(m_resolution * 3);

		m_vertices.emplace_back(glm::vec2{ m_radius * 0.5f, m_radius * 0.5f });

		for (uint32_t i = 0; i < m_resolution; ++i)
		{
			float angle = static_cast<float>(i) * 2.0f * PI / static_cast<float>(m_resolution);

			m_vertices.emplace_back(glm::vec2{ cos(angle) * m_radius + m_radius, sin(angle) * m_radius + m_radius });

			m_indices.push_back(0);
			m_indices.push_back(i + 1);
			m_indices.push_back(i + 2);
		}

		m_indices.back() = 1;
	}

	void circle_shape::gen_outline_vertices()
	{
		if (m_radius == 0.0f) return;

		m_outline_vertices.clear();
		m_outline_indices.clear();

		m_outline_vertices.reserve(m_resolution * 2);
		m_outline_indices.reserve(m_resolution * 6);

		float aspect = m_outline_thickness / m_radius;

		for (uint32_t i = 0; i < m_resolution; ++i)
		{
			vertex_2d& v = m_vertices[i + 1];

			float angle = static_cast<float>(i) * 2.0f * PI / static_cast<float>(m_resolution);
			m_outline_vertices.emplace_back(glm::vec2{ cos(angle) * m_radius + m_radius, sin(angle) * m_radius + m_radius });
			m_outline_vertices.push_back(v);
			
			uint32_t index = i * 2;
			m_outline_indices.push_back(index);
			m_outline_indices.push_back(index + 1);
			m_outline_indices.push_back(index + 2);
			m_outline_indices.push_back(index + 2);
			m_outline_indices.push_back(index + 3);
			m_outline_indices.push_back(index + 1);
		}

		auto last_index = m_outline_indices.size();
		m_outline_indices[last_index - 4] = 0;
		m_outline_indices[last_index - 3] = 0;
		m_outline_indices[last_index - 2] = 1;
	}

	void circle_shape::update_fill_color()
	{
		for (auto& v : m_vertices) v.vertex_color = m_fill_color;
	}

	void circle_shape::update_outline_color()
	{
		for (auto& v : m_outline_vertices) v.vertex_color = m_outline_color;
	}

	void circle_shape::scale_for_outline()
	{
		float inner_radius = m_radius - m_outline_thickness;

		for (uint32_t i = 0; i < m_resolution; ++i)
		{
			float angle = static_cast<float>(i) * 2.0f * PI / static_cast<float>(m_resolution);

			auto index = i + 1;
			m_vertices[index].position = glm::vec2{ cos(angle) * inner_radius + m_radius, sin(angle) * inner_radius + m_radius };
		}
	}
}
#include "graphics/render_target.h"

#include <array>

#include <glad/glad.h>

#include "graphics/render_states.h"
#include "graphics/texture.h"
#include "graphics/drawable.h"
#include "graphics/render_states_cache.h"

#include "utility/gl_check.h"

namespace age
{
	constexpr inline uint32_t factor_to_GL_constant(blend_mode::factor factor)
	{
		switch (factor)
		{
			case blend_mode::factor::zero:					return GL_ZERO;
			case blend_mode::factor::one:					return GL_ONE;
			case blend_mode::factor::src_color:				return GL_SRC_COLOR;
			case blend_mode::factor::one_minus_src_color:	return GL_ONE_MINUS_SRC_COLOR;
			case blend_mode::factor::dst_color:				return GL_DST_COLOR;
			case blend_mode::factor::one_minus_dst_color:	return GL_ONE_MINUS_DST_COLOR;
			case blend_mode::factor::src_alpha:				return GL_SRC_ALPHA;
			case blend_mode::factor::one_minus_src_alpha:	return GL_ONE_MINUS_SRC_ALPHA;
			case blend_mode::factor::dst_alpha:				return GL_DST_ALPHA;
			case blend_mode::factor::one_minus_dst_alpha:	return GL_ONE_MINUS_DST_ALPHA;
		}

		return GL_ZERO;
	}

	constexpr inline uint32_t equation_to_GL_constant(blend_mode::equation equation)
	{
		switch (equation)
		{
			case blend_mode::equation::add:					return GL_FUNC_ADD;
			case blend_mode::equation::subtract:			return GL_FUNC_SUBTRACT;
			case blend_mode::equation::reverse_subtract:	return GL_FUNC_REVERSE_SUBTRACT;
			/*
			* OpenGL ES 3
			case blend_mode::equation::min:					return GL_MIN;
			case blend_mode::equation::max:					return GL_MAX;
			*/
		}

		return GL_FUNC_ADD;
	}

	constexpr inline GLenum primitive_type_to_GL_constant(primitive_type value)
	{
		constexpr std::array<GLenum, 6> primitive_arr{GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN};

		return primitive_arr[static_cast<uint32_t>(value)];
	}

	render_target::render_target()
		: m_projection_needs_update{ true }
	{}
	
	int_rect render_target::get_viewport(const view_2d& view) const
	{
		auto size = get_size();

		float            width = static_cast<float>(size.x);
		float            height = static_cast<float>(size.y);
		const float_rect& viewport = view.get_viewport();

		return int_rect({ static_cast<int>(0.5f + width * viewport.left), static_cast<int>(0.5f + height * viewport.top) },
						{ static_cast<int>(0.5f + width * viewport.width), static_cast<int>(0.5f + height * viewport.height) });
	}

	glm::vec2 render_target::map_pixel_to_coords(const glm::vec2& point) const
	{
		glm::vec2 normalized;

		float_rect viewport = float_rect{ m_viewport };
		normalized.x = -1.f + 2.f * (point.x - viewport.left) / viewport.width;
		normalized.y = 1.f - 2.f * (point.y - viewport.top) / viewport.height;

		return glm::vec2{ get_inverse_projection() * glm::vec4{ normalized.x, normalized.y, 0.0f, 1.0f } };
	}

	glm::vec2 render_target::map_coords_to_pixel(const glm::vec2& point) const
	{
		auto normalized = glm::vec2{ m_projection_matrix * glm::vec4{ point.x, point.y, 0.0f, 1.0f } };

		// Then convert to viewport coordinates
		glm::vec2  pixel;
		auto viewport = float_rect{m_viewport};
		pixel.x = (normalized.x + 1.0f) / 2.0f * viewport.width + viewport.left;
		pixel.y = (-normalized.y + 1.0f) / 2.0f * viewport.height + viewport.top;

		return pixel;
	}

	glm::vec2 render_target::map_normalized_to_coords(const glm::vec2 &point) const
	{
		return map_pixel_to_coords(map_normalized_to_pixel(point));
	}

	glm::vec2 render_target::map_normalized_to_pixel(const glm::vec2& point) const
	{
		auto viewport = float_rect{ m_viewport };

		glm::vec2 pixel_pos;
		pixel_pos.x = point.x * viewport.width + viewport.left;
		pixel_pos.y = point.y * viewport.height + viewport.top;

		return pixel_pos;
	}

	void render_target::apply_view(const view_2d& value)
	{
		// Set the viewport
		m_viewport = get_viewport(value);
		m_projection_matrix = value.get_transform();
		m_projection_needs_update = true;
		m_view_size = value.get_size();

		restore_view();
	}

	void render_target::restore_view()
	{
		// 1. Sync Viewport
		// We calculate the OpenGL-style bottom-left 'top' using the current target's height
		int gl_y = static_cast<int>(get_size().y) - (m_viewport.top + m_viewport.height);
		int_rect current_view_rect{ {m_viewport.left, gl_y}, {m_viewport.width, m_viewport.height} };

		if (current_view_rect != g_render_state.view)
		{
			GL_CALL(glViewport(current_view_rect.left, current_view_rect.top,
							   current_view_rect.width, current_view_rect.height));
			g_render_state.view = current_view_rect;
		}

		// 2. Sync Projection Matrix (UBO)
		if (m_projection_matrix != g_render_state.projection_matrix)
		{
			engine::get_instance()->get_vp_matrix_ubo().buffer_sub_data(0, sizeof(glm::mat4), &m_projection_matrix);
			g_render_state.projection_matrix = m_projection_matrix;
		}
	}

	const glm::vec2& render_target::get_view_size() const
	{
		return m_view_size;
	}

	void render_target::draw(const drawable& drawable_object, const render_states& states)
	{
		drawable_object.draw(*this, states);
	}

	void render_target::draw(const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states)
	{
		if (!vertices || !indices || !num_indices)
			return;

		prepare_draw(vertices, num_vertices, indices, num_indices, states);
		GL_CALL(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(num_indices), GL_UNSIGNED_INT, 0));
	}

	void render_target::draw(const vertex_2d vertices[], size_t num_vertices, primitive_type type, const render_states& states)
	{
		if (!vertices || !num_vertices)
			return;

		prepare_draw(vertices, num_vertices, states);
		GL_CALL(glDrawArrays(primitive_type_to_GL_constant(type), 0, static_cast<GLsizei>(num_vertices)));
	}

	void render_target::init()
	{
		GL_CALL(glEnable(GL_BLEND));
		apply_blend_mode(blend_mode::blend_alpha);
	}

	void render_target::prepare_draw(const vertex_2d vertices[], size_t num_vertices, const render_states& states)
	{
		auto& program = states.get_shader_program();

		program.bind();

		engine::get_instance()->get_model_matrix_ubo().buffer_sub_data(0, sizeof(glm::mat4), &states.get_transform());

		states.get_texture().bind();

		engine::get_instance()->get_default_vertex_buffer_object().update_data(vertices, num_vertices * sizeof(vertex_2d), vertex_buffer_object::usage::stream_draw);

		apply_blend_mode(states.get_blend_mode());
	}

	void render_target::prepare_draw(const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states)
	{
		engine::get_instance()->get_default_element_buffer_object().update_data(indices, num_indices * sizeof(uint32_t), vertex_buffer_object::usage::stream_draw);

		prepare_draw(vertices, num_vertices, states);
	}

	void render_target::apply_blend_mode(const blend_mode& mode)
	{
		if (mode != m_states_cache.last_blend_mode)
		{
			GL_CALL(glBlendFuncSeparate(factor_to_GL_constant(mode.color_src_factor),
				factor_to_GL_constant(mode.color_dst_factor),
				factor_to_GL_constant(mode.alpha_src_factor),
				factor_to_GL_constant(mode.alpha_dst_factor)));

			GL_CALL(glBlendEquationSeparate(equation_to_GL_constant(mode.color_equation),
				equation_to_GL_constant(mode.alpha_equation)));

			m_states_cache.last_blend_mode = mode;
		}
	}

	const glm::mat4& render_target::get_inverse_projection() const
	{
		if (m_projection_needs_update)
		{
			m_projection_matrix_inverse = glm::inverse(m_projection_matrix);
			m_projection_needs_update = false;
		}

		return m_projection_matrix_inverse;
	}
}
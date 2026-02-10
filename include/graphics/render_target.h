#pragma once

#include "blend_mode.h"
#include "vertex_2d.h"
#include "view_2d.h"
#include "vertex_array_object.h"
#include "vertex_buffer_object.h"

namespace age
{
	class drawable;
	class render_states;
	class texture;

	enum class primitive_type : uint32_t
	{
		points = 0,
		lines = 1,
		line_strip = 2,
		triangles = 3,
		triangle_strip = 4,
		triangle_fan = 5
	};

	class render_target
	{
	public:
		render_target();
		virtual ~render_target() = default;

		render_target(const render_target& other) = default;
		render_target(render_target&& other) = default;
		
		render_target& operator = (const render_target& other) = default;
		render_target& operator = (render_target&& other) = default;

	public:
		int_rect get_viewport(const view_2d& view) const;

		glm::vec2 map_pixel_to_coords(const glm::vec2& point) const;
		glm::vec2 map_coords_to_pixel(const glm::vec2& point) const;
		glm::vec2 map_normalized_to_coords(const glm::vec2& point) const;
		glm::vec2 map_normalized_to_pixel(const glm::vec2& point) const;
		
		virtual glm::u32vec2 get_size() const = 0;

		void apply_view(const view_2d& view);
		void restore_view();
		const glm::vec2& get_view_size() const;

		virtual void draw(const drawable& drawable_object, const render_states& states);
		virtual void draw(const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states);
		virtual void draw(const vertex_2d vertices[], size_t num_vertices, primitive_type type, const render_states& states);

	protected:
		void init();

	private:
		struct states_cache
		{
			blend_mode last_blend_mode = blend_mode::blend_none;
			const texture* last_texture = nullptr;
		};

		void prepare_draw(const vertex_2d vertices[], size_t num_vertices, const render_states& states);
		void prepare_draw(const vertex_2d vertices[], size_t num_vertices, const uint32_t indices[], size_t num_indices, const render_states& states);
		void apply_blend_mode(const blend_mode& mode);

		const glm::mat4& get_inverse_projection() const;

		glm::vec2 m_view_size{ 0.0f };
		int_rect m_viewport;
		glm::mat4 m_projection_matrix{ 1.0f };
		mutable glm::mat4 m_projection_matrix_inverse{ 1.0f };
		states_cache m_states_cache;

		mutable bool m_projection_needs_update;
	};
}
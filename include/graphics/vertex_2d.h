#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "color.h"

namespace age
{
	struct vertex_2d
	{
		vertex_2d()
			: vertex_color{ 255, 255, 255 }
		{}

		vertex_2d(glm::vec2 p_position)
			: position{ p_position }
			, vertex_color{ 255, 255, 255 }
		{}

		vertex_2d(glm::vec2 p_position, glm::vec2 p_tex_coords)
			: position{ p_position }
			, vertex_color{ 255, 255, 255 }
			, tex_coords{ glm::vec3{p_tex_coords, 0.0f } }
		{}

		vertex_2d(glm::vec2 p_position, glm::vec3 p_tex_coords)
			: position{ p_position }
			, vertex_color{ 255, 255, 255 }
			, tex_coords{ p_tex_coords }
		{}

		vertex_2d(glm::vec2 p_position, color p_color, glm::vec2 p_tex_coords)
			: position{ p_position }
			, vertex_color{ p_color }
			, tex_coords{ glm::vec3{ p_tex_coords, 0.0f } }
		{}

		vertex_2d(glm::vec2 p_position, color p_color, glm::vec3 p_tex_coords)
			: position{ p_position }
			, vertex_color{ p_color }
			, tex_coords{ p_tex_coords }
		{}

		glm::vec2 position{};
		color vertex_color{};
		glm::vec3 tex_coords{};
	};
}
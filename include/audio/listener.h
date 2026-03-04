#pragma once

#include <glm/vec3.hpp>

namespace age::listener
{
	void set_volume(float value);
	float get_volume();

	void set_position(const glm::vec3& value);
	const glm::vec3& get_position();

	void set_direction(const glm::vec3& value);
	const glm::vec3& get_direction();

	void set_up_vector(const glm::vec3& value);
	const glm::vec3& get_up_vector();
}

#include "audio/listener.h"

#include "audio/device.h"

namespace age::audio
{
	namespace listener
	{
		void set_volume(float value)
		{
			device::set_listener_volume(value);
		}

		float get_volume()
		{
			return device::get_listener_volume();
		}

		void set_position(const glm::vec3& value)
		{
			device::set_listener_position(value);
		}

		const glm::vec3& get_position()
		{
			return device::get_listener_position();
		}

		void set_direction(const glm::vec3& value)
		{
			device::set_listener_direction(value);
		}

		const glm::vec3& get_direction()
		{
			return device::get_listener_direction();
		}

		void set_up_vector(const glm::vec3& value)
		{
			device::set_listener_up_vector(value);
		}

		const glm::vec3& get_up_vector()
		{
			return device::get_listener_up_vector();
		}
	}
}
#pragma once

#include <glm/glm.hpp>

namespace Tracer
{
	class Camera
	{
	public:
		Camera(glm::vec3 eye, glm::vec3 lookAt, float fov);
		~Camera() = default;
	};
}

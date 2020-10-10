#pragma once

#include <glm/glm.hpp>

namespace Uniforms
{
	struct MVP
	{
		glm::mat4 view = glm::mat4(1.f);
		glm::mat4 projection = glm::mat4(1.f);
		glm::vec3 direction{};
	};
}

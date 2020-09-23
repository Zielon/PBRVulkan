#pragma once

#include <glm/glm.hpp>

namespace Uniforms
{
	struct MVP
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}

#pragma once

#include <glm/glm.hpp>

namespace Uniforms
{
	struct MVP
	{
		alignas(16) glm::mat4 Model;
		alignas(16) glm::mat4 View;
		alignas(16) glm::mat4 Projection;
	};
}

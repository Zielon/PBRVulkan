#pragma once

#include <glm/glm.hpp>

namespace Geometry
{
	struct MVP
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}

#pragma once

#include <glm/glm.hpp>

namespace Assets
{
	enum LightType
	{
		QuadLight,
		SphereLight
	};

	struct Light
	{
		glm::vec3 position;
		glm::vec3 emission;
		glm::vec3 u;
		glm::vec3 v;
		float radius;
		float area;
		float type;
	};
}

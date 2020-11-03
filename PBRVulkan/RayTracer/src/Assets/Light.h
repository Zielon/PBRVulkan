#pragma once

#include <glm/glm.hpp>

namespace Assets
{
	enum LightType
	{
		QuadLight,
		SphereLight
	};

	struct alignas(16) Light final
	{
		glm::vec4 position{};
		glm::vec4 emission{};
		glm::vec4 u{};
		glm::vec4 v{};
		glm::vec4 area{};
	};
}

#pragma once

#include <glm/glm.hpp>

namespace Loader
{
	struct RenderOptions
	{
		RenderOptions()
		{
			maxDepth = 2;
			useEnvMap = false;
			resolution = glm::vec2(1920, 1080);
			hdrMultiplier = 1.0f;
		}

		glm::ivec2 resolution{};
		int maxDepth;
		bool useEnvMap;
		float hdrMultiplier;
	};
}

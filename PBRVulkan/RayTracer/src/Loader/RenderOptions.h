#pragma once

#include <glm/glm.hpp>

namespace Loader
{
	struct RenderOptions
	{
		RenderOptions()
		{
			maxDepth = 2;
			numTilesX = 5;
			numTilesY = 5;
			useEnvMap = false;
			resolution = glm::vec2(1280, 720);
			hdrMultiplier = 1.0f;
		}

		glm::ivec2 resolution{};
		int maxDepth;
		int numTilesX;
		int numTilesY;
		bool useEnvMap;
		float hdrMultiplier;
	};
}

#pragma once

#include <glm/glm.hpp>

namespace Uniforms
{
	struct alignas(16) Compute final
	{
		glm::uint32_t iteration;
		glm::float32_t colorPhi{};
		glm::float32_t normalPhi{};
		glm::float32_t positionPhi{};
		glm::float32_t stepWidth{};
	};
}

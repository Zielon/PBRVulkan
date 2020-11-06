#pragma once

#include <glm/glm.hpp>

namespace Uniforms
{
	struct alignas(16) Global final
	{
		glm::mat4 view = glm::mat4(1.f);
		glm::mat4 projection = glm::mat4(1.f);
		glm::mat4 normalMat = glm::mat4(1.f);
		glm::vec3 direction{};
		uint32_t lights{};
		uint32_t hasHDR{};
		uint32_t ssp{};
		uint32_t maxDepth{};
		uint32_t frame{};
		float aperture{};
		float focalDistance{};
		float hdrResolution{};
		float AORayLength{};
		float denoiserStrength{};
	};
}

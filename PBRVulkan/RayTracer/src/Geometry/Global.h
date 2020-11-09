#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Uniforms
{
	struct Global final
	{
		glm::mat4 view = glm::mat4(1.f);
		glm::mat4 projection = glm::mat4(1.f);
		alignas(16) glm::vec3 cameraPos{};
		glm::uint32_t lights{};
		glm::uint32_t hasHDR{};
		glm::uint32_t ssp{};
		glm::uint32_t maxDepth{};
		glm::uint32_t frame{};
		glm::float32_t aperture{};
		glm::float32_t focalDistance{};
		glm::float32_t hdrResolution{};
		glm::float32_t AORayLength{};
		glm::float32_t denoiserStrength{};
		glm::int32_t integratorType{};
	};
}

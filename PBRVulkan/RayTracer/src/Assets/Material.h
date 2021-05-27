#pragma once

#include <glm/glm.hpp>

namespace Assets
{
	struct alignas(16) Material final
	{
		Material()
		{
			albedo = glm::vec4(1.0f, 1.0f, 1.0f, 0.f);
			extinction = glm::vec4(1.0f, 1.0f, 1.0f, 1.f);
			emission = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			metallic = 0.0f;
			roughness = 0.5f;
			ior = 1.45f;
			atDistance = 1.f;
			albedoTexID = -1;
			metallicRoughnessTexID = -1;
			normalmapTexID = -1;
		};

		glm::vec4 albedo{};
		glm::vec4 emission{};
		glm::vec4 extinction{};

		glm::float32_t metallic;
		glm::float32_t roughness;
		glm::float32_t subsurface;
		glm::float32_t specularTint;

		glm::float32_t sheen{};
		glm::float32_t sheenTint{};
		glm::float32_t clearcoat{};
		glm::float32_t clearcoatGloss{};

		glm::float32_t transmission{};
		glm::float32_t ior;
		glm::float32_t atDistance;

		glm::int32_t albedoTexID;
		glm::int32_t metallicRoughnessTexID;
		glm::int32_t normalmapTexID;
	};
}

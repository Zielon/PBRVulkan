#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Assets
{
	enum MaterialType
	{
		DISNEY,
		GLASS
	};

	struct Material final
	{
		Material()
		{
			albedo = glm::vec4(1.0f, 1.0f, 1.0f, DISNEY);
			emission = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			metallic = 0.0f;
			roughness = 0.5f;
			ior = 1.45f;
			transmittance = 0.0f;
			albedoTexID = -1;
			metallicRoughnessTexID = -1;
			normalmapTexID = -1;
			heightmapTexID = -1;
		};

		glm::vec4 albedo{};
		glm::vec4 emission{};
		glm::float32_t metallic;
		glm::float32_t roughness;
		glm::float32_t ior;
		glm::float32_t transmittance;
		glm::int32_t albedoTexID;
		glm::int32_t metallicRoughnessTexID;
		glm::int32_t normalmapTexID;
		glm::int32_t heightmapTexID;
	};
}

#pragma once

#include <glm/glm.hpp>

namespace Assets
{
	enum MaterialType
	{
		DISNEY,
		GLASS
	};

	// Material data structure layout is 32 * 4 bytes
	struct alignas(16) Material final
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

		// 32 bytes
		glm::vec4 albedo{};

		// 32 bytes
		glm::vec4 emission{};

		// 32 bytes
		float metallic;
		float roughness;
		float ior;
		float transmittance;

		// 32 bytes
		int32_t albedoTexID;
		int32_t metallicRoughnessTexID;
		int32_t normalmapTexID;
		int32_t heightmapTexID;
	};
}

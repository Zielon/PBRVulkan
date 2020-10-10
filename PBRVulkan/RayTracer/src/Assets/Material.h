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
	class Material
	{
	public:
		Material()
		{
			albedo = glm::vec3(1.0f, 1.0f, 1.0f);
			materialType = DISNEY;
			emission = glm::vec3(0.0f, 0.0f, 0.0f);
			metallic = 0.0f;
			roughness = 0.5f;
			ior = 1.45f;
			transmittance = 0.0f;
			albedoTexID = -1.0f;
			metallicRoughnessTexID = -1.0f;
			normalmapTexID = -1.0f;
			heightmapTexID = -1.0f;
		};

		// 32 bytes
		glm::vec3 albedo{};
		float materialType;

		// 32 bytes
		glm::vec3 emission{};
		float unused{};

		// 32 bytes
		float metallic;
		float roughness;
		float ior;
		float transmittance;

		// 32 bytes
		float albedoTexID;
		float metallicRoughnessTexID;
		float normalmapTexID;
		float heightmapTexID;
	};
}

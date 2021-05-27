/*
Copyright (c) 2018 Miles Macklin

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

/* 
	This is modified version of the original code 
	Link to original code: https://github.com/mmacklin/tinsel
*/

#include "Loader.h"

#include <iostream>

#include "RenderOptions.h"

#include "../Tracer/Scene.h"
#include "../Assets/Light.h"
#include "../Assets/Material.h"
#include "../Assets/Mesh.h"

namespace Loader
{
	static const float _PI = 3.14159265358979323846f;

	static const int kMaxLineLength = 2048;

	bool LoadSceneFromFile(const std::string& filename, SceneBase& scene, RenderOptions& renderOptions)
	{
		FILE* file;
		file = fopen(filename.c_str(), "r");

		if (!file)
		{
			printf("[ERROR] Couldn't open %s for reading\n", filename.c_str());
			return false;
		}

		printf("[LOADER] Scene processing has begun!\n");

		struct MaterialData
		{
			Assets::Material mat;
			int id;
		};

		std::map<std::string, MaterialData> materialMap;
		std::vector<std::string> albedoTex;
		std::vector<std::string> metallicRoughnessTex;
		std::vector<std::string> normalTex;

		char line[kMaxLineLength];

		//Defaults
		Assets::Material defaultMat;
		scene.AddMaterial(defaultMat);

		bool cameraAdded = false;

		while (fgets(line, kMaxLineLength, file))
		{
			// skip comments
			if (line[0] == '#')
				continue;

			// name used for materials and meshes
			char name[kMaxLineLength] = {0};

			//--------------------------------------------
			// Material

			if (sscanf(line, " material %s", name) == 1)
			{
				Assets::Material material{};

				char albedoTexName[100] = "None";
				char metallicRoughnessTexName[100] = "None";
				char normalTexName[100] = "None";

				while (fgets(line, kMaxLineLength, file))
				{
					// end group
					if (strchr(line, '}'))
						break;

					sscanf(line, " name %s", name);
					sscanf(line, " color %f %f %f", &material.albedo.x, &material.albedo.y, &material.albedo.z);
					sscanf(line, " emission %f %f %f", &material.emission.x, &material.emission.y,
					       &material.emission.z);
					sscanf(line, " metallic %f", &material.metallic);
					sscanf(line, " roughness %f", &material.roughness);
					sscanf(line, " subsurface %f", &material.subsurface);
					sscanf(line, " specular %f", &material.albedo.w);
					sscanf(line, " specularTint %f", &material.specularTint);
					sscanf(line, " anisotropic %f", &material.emission.w);
					sscanf(line, " sheen %f", &material.sheen);
					sscanf(line, " sheenTint %f", &material.sheenTint);
					sscanf(line, " clearcoat %f", &material.clearcoat);
					sscanf(line, " clearcoatGloss %f", &material.clearcoatGloss);
					sscanf(line, " transmission %f", &material.transmission);
					sscanf(line, " ior %f", &material.ior);
					sscanf(line, " extinction %f %f %f", &material.extinction.x, &material.extinction.y,
					       &material.extinction.z);
					sscanf(line, " atDistance %f", &material.atDistance);

					sscanf(line, " albedoTexture %s", albedoTexName);
					sscanf(line, " metallicRoughnessTexture %s", metallicRoughnessTexName);
					sscanf(line, " normalTexture %s", normalTexName);
				}

				// Albedo Texture
				if (strcmp(albedoTexName, "None") != 0)
					material.albedoTexID = scene.AddTexture(albedoTexName);

				// MetallicRoughness Texture
				if (strcmp(metallicRoughnessTexName, "None") != 0)
					material.metallicRoughnessTexID = scene.AddTexture(metallicRoughnessTexName);

				// Normal Map Texture
				if (strcmp(normalTexName, "None") != 0)
					material.normalmapTexID = scene.AddTexture(normalTexName);

				// Add material to map
				if (materialMap.find(name) == materialMap.end()) // New material
				{
					int id = scene.AddMaterial(material);
					materialMap[name] = MaterialData{material, id};
				}
			}

			//--------------------------------------------
			// Light

			if (strstr(line, "light"))
			{
				Assets::Light light{};
				glm::vec3 v1{};
				glm::vec3 v2{};
				char light_type[20] = "None";

				while (fgets(line, kMaxLineLength, file))
				{
					// end group
					if (strchr(line, '}'))
						break;

					sscanf(line, " position %f %f %f", &light.position.x, &light.position.y, &light.position.z);
					sscanf(line, " emission %f %f %f", &light.emission.x, &light.emission.y, &light.emission.z);

					sscanf(line, " radius %f", &light.radius);
					sscanf(line, " v1 %f %f %f", &v1.x, &v1.y, &v1.z);
					sscanf(line, " v2 %f %f %f", &v2.x, &v2.y, &v2.z);
					sscanf(line, " type %s", light_type);
				}

				if (strcmp(light_type, "Quad") == 0)
				{
					light.type = Assets::LightType::QuadLight;
					light.u = v1 - light.position;
					light.v = v2 - light.position;
					light.area = length(cross(light.u.xyz(), light.v.xyz()));
				}
				else if (strcmp(light_type, "Sphere") == 0)
				{
					light.type = Assets::LightType::SphereLight;
					light.area = 4.0f * _PI * light.radius * light.radius;
				}

				scene.AddLight(light);
			}

			//--------------------------------------------
			// Renderer

			if (strstr(line, "Renderer"))
			{
				char envMap[200] = "None";

				while (fgets(line, kMaxLineLength, file))
				{
					// end group
					if (strchr(line, '}'))
						break;

					sscanf(line, " envMap %s", &envMap);
					sscanf(line, " resolution %d %d", &renderOptions.resolution.x, &renderOptions.resolution.y);
					sscanf(line, " hdrMultiplier %f", &renderOptions.hdrMultiplier);
					sscanf(line, " maxDepth %i", &renderOptions.maxDepth);
				}

				if (strcmp(envMap, "None") != 0)
				{
					scene.AddHDR(envMap);
					renderOptions.useEnvMap = true;
				}
			}

			//--------------------------------------------
			// Camera

			if (strstr(line, "Camera"))
			{
				glm::vec3 position{};
				glm::vec3 lookAt{};
				float fov{};
				float aperture = 0, focalDist = 1;

				while (fgets(line, kMaxLineLength, file))
				{
					// end group
					if (strchr(line, '}'))
						break;

					sscanf(line, " position %f %f %f", &position.x, &position.y, &position.z);
					sscanf(line, " lookAt %f %f %f", &lookAt.x, &lookAt.y, &lookAt.z);
					sscanf(line, " aperture %f ", &aperture);
					sscanf(line, " focaldist %f", &focalDist);
					sscanf(line, " fov %f", &fov);
				}

				float aspect = static_cast<float>(renderOptions.resolution.x) / renderOptions.resolution.y;
				scene.AddCamera(position, lookAt, fov, aspect);
				cameraAdded = true;
			}


			//--------------------------------------------
			// Mesh

			if (strstr(line, "mesh"))
			{
				std::string filename;
				glm::vec3 pos{};
				glm::vec3 scale{};
				auto xform = glm::mat4(1.f);
				int material_id = 0; // Default Material ID
				while (fgets(line, kMaxLineLength, file))
				{
					// end group
					if (strchr(line, '}'))
						break;

					char file[2048];
					char matName[100];

					if (sscanf(line, " file %s", file) == 1)
					{
						filename = std::string(file);
					}

					if (sscanf(line, " material %s", matName) == 1)
					{
						// look up material in dictionary
						if (materialMap.find(matName) != materialMap.end())
						{
							material_id = materialMap[matName].id;
						}
						else
						{
							printf("Could not find material %s\n", matName);
						}
					}

					sscanf(line, " position %f %f %f", &xform[3][0], &xform[3][1], &xform[3][2]);
					sscanf(line, " scale %f %f %f", &xform[0][0], &xform[1][1], &xform[2][2]);
				}
				if (!filename.empty())
				{
					int mesh_id = scene.AddMesh(filename);
					if (mesh_id != -1)
					{
						Assets::MeshInstance instance(mesh_id, xform, material_id);
						scene.AddMeshInstance(instance);
					}
				}
			}
		}

		fclose(file);

		// Add default camera if none was specified
		if (!cameraAdded)
			scene.AddCamera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -10.0f), 35.0f, 1.f);

		return true;
	}
}

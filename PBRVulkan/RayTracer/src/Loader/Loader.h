#pragma once

#include <string>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>

namespace Assets
{
	struct Light;
	class Material;
	class Mesh;
	class MeshInstance;
}

namespace Tracer
{
	class Scene;
}

namespace Loader
{
	class SceneBase
	{
	public:
		virtual ~SceneBase() {};
		virtual void AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov) = 0;
		virtual void AddHDR(const std::string& path) = 0;
		virtual int AddMesh(const std::string& path) = 0;
		virtual int AddTexture(const std::string& path) = 0;
		virtual int AddMaterial(Assets::Material material) = 0;
		virtual int AddLight(Assets::Light light) = 0;
		virtual int AddMeshInstance(class Assets::MeshInstance meshInstance) = 0;
	};

	bool LoadSceneFromFile(
		const std::string& filename,
		SceneBase& scene,
		struct RenderOptions& renderOptions);

	extern int (*Log)(const char* szFormat, ...);
}

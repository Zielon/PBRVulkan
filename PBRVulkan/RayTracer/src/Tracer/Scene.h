#pragma once

#include <map>
#include <memory>
#include <vector>
#include <string>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>

#include "../Assets/Mesh.h"
#include "../Geometry/Vertex.h"
#include "../Loader/Loader.h"

#include "../Vulkan/CommandBuffers.h"
#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;
	class Buffer;
	class CommandPool;
	class Image;
}

namespace Assets
{
	class MeshInstance;
	class Mesh;
	class Texture;
	class Material;
	struct Light;
}

namespace Tracer
{
	class Scene final : public Loader::SceneBase
	{
	public:
		NON_COPIABLE(Scene)

		Scene(const std::string& config, const class Vulkan::Device& device, const Vulkan::CommandPool& commandPool);
		~Scene();

		void AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov) override;
		void AddHDR(const std::string& path) override;
		int AddMesh(const std::string& path) override;
		int AddTexture(const std::string& path) override;
		int AddMaterial(Assets::Material material) override;
		int AddLight(Assets::Light light) override;
		int AddMeshInstance(class Assets::MeshInstance meshInstance) override;

		[[nodiscard]] const std::vector<std::unique_ptr<Assets::Mesh>>& GetMeshes() const
		{
			return meshes;
		}

		[[nodiscard]] class Camera& GetCamera() const
		{
			return *camera;
		}

		[[nodiscard]] const class Vulkan::Buffer& GetVertexBuffer() const
		{
			return *vertexBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer& GetIndexBuffer() const
		{
			return *indexBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer& GetMaterialBuffer() const
		{
			return *materialBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer& GetOffsetBuffer() const
		{
			return *offsetBuffer;
		}

		[[nodiscard]] const std::vector<std::unique_ptr<class TextureImage>>& GetTextures() const
		{
			return textureImages;
		}

		[[nodiscard]] uint32_t GetTextureSize() const
		{
			return textureImages.size();
		}

		[[nodiscard]] uint32_t GetIndexSize() const
		{
			return indeciesSize;
		}

		[[nodiscard]] uint32_t GetVertexSize() const
		{
			return verticesSize;
		}

	private:
		std::string config;
		uint32_t verticesSize{};
		uint32_t indeciesSize{};
		std::unique_ptr<class Camera> camera;

		const class Vulkan::Device& device;
		const class Vulkan::CommandPool& commandPool;

		// Assets
		std::map<std::string, int> meshMap;
		std::map<std::string, int> textureMap;

		std::vector<std::unique_ptr<Assets::Mesh>> meshes;
		std::vector<std::unique_ptr<Assets::Texture>> textures;
		std::vector<std::unique_ptr<TextureImage>> textureImages;

		std::vector<Assets::MeshInstance> meshInstances;
		std::vector<Assets::Material> materials;
		std::vector<Assets::Light> lights;

		std::unique_ptr<class Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<class Vulkan::Buffer> indexBuffer;
		std::unique_ptr<class Vulkan::Buffer> materialBuffer;
		std::unique_ptr<class Vulkan::Buffer> offsetBuffer;

		std::unique_ptr<class Vulkan::Image> image;

		void Load();
		void Process();
		void CreateBuffers();
	};
}

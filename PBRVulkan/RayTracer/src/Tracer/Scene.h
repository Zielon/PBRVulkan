#pragma once

#include <map>
#include <memory>
#include <vector>
#include <string>

#define GLM_FORCE_SWIZZLE 
#include <future>
#include <glm/glm.hpp>

#include "Application.h"

#include "../Geometry/Vertex.h"
#include "../Loader/Loader.h"
#include "../Vulkan/Vulkan.h"
#include "../Assets/Light.h"
#include "../Loader/RenderOptions.h"

#include "../3rdParty/HDRLoader.h"

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
	class HDRData;
	struct Light;
}

namespace Tracer
{
	class Scene final : public Loader::SceneBase
	{
	public:
		NON_COPIABLE(Scene)

		Scene(
			std::string config,
			const class Vulkan::Device& device,
			const Vulkan::CommandPool& commandPool);
		~Scene() override;

		void AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov, float aspect) override;
		void AddHDR(const std::string& path) override;
		int AddMesh(const std::string& path) override;
		int AddTexture(const std::string& path) override;
		int AddMaterial(Assets::Material material) override;
		int AddLight(Assets::Light light) override;
		int AddMeshInstance(class Assets::MeshInstance meshInstance) override;
		void CreateBuffers();

		[[nodiscard]] const std::vector<std::unique_ptr<Assets::Mesh>>& GetMeshes() const
		{
			return meshes;
		}

		[[nodiscard]] const std::vector<Assets::MeshInstance>& GetMeshInstances() const
		{
			return meshInstances;
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

		[[nodiscard]] const Vulkan::Buffer& GetLightsBuffer() const
		{
			return *lightsBuffer;
		}

		[[nodiscard]] const std::vector<std::unique_ptr<class TextureImage>>& GetTextures() const
		{
			return textureImages;
		}

		[[nodiscard]] const std::vector<std::unique_ptr<class TextureImage>>& GetHDRTextures() const
		{
			return hdrImages;
		}

		[[nodiscard]] bool UseHDR() const
		{
			return !hdrImages.empty();
		}

		[[nodiscard]] uint32_t GetLightsSize() const
		{
			return lights.size();
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

		[[nodiscard]] glm::float32 GetHDRResolution() const
		{
			return hdrResolution;
		}

		[[nodiscard]] Loader::RenderOptions GetRendererOptions() const
		{
			return options;
		}

	private:
		std::string config;
		const std::string root = "../Assets/Scenes/";
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
		std::vector<std::unique_ptr<TextureImage>> hdrImages;

		std::vector<Assets::MeshInstance> meshInstances;
		std::vector<Assets::Material> materials;
		std::vector<Assets::Light> lights;

		std::unique_ptr<class Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<class Vulkan::Buffer> indexBuffer;
		std::unique_ptr<class Vulkan::Buffer> materialBuffer;
		std::unique_ptr<class Vulkan::Buffer> offsetBuffer;
		std::unique_ptr<class Vulkan::Buffer> lightsBuffer;
		std::unique_ptr<class Vulkan::Image> image;

		std::future<void> hdrLoader{};
		float hdrResolution{};

		Loader::RenderOptions options;

		void Wait();
		void Print() const;
		void Load();
		void LoadEmptyBuffers();
		void LoadHDR(HDRData* hdr);
		void Fill(std::unique_ptr<class Vulkan::Buffer>& buffer, void* data, size_t size,
		          VkBufferUsageFlagBits storage,
		          VkMemoryAllocateFlags allocateFlags) const;
	};
}

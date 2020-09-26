#pragma once

#include <memory>


#include "../Geometry/Vertex.h"
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
	class TextureImage;
}

namespace Tracer
{
	class Scene final
	{
	public:
		NON_COPIABLE(Scene)

		Scene(const class Vulkan::Device& device, const Vulkan::CommandPool& commandPool);
		~Scene();

		[[nodiscard]] const class Vulkan::Buffer& GetVertexBuffer() const
		{
			return *vertexBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer& GetIndexBuffer() const
		{
			return *indexBuffer;
		}

		[[nodiscard]] const Assets::TextureImage& GetTexture() const
		{
			return *textureImage;
		}

		[[nodiscard]] uint32_t GetIndexSize() const
		{
			return static_cast<uint32_t>(indices.size());
		}

		[[nodiscard]] uint32_t GetVertexSize() const
		{
			return static_cast<uint32_t>(vertices.size());
		}

	private:
		const class Vulkan::Device& device;
		const class Vulkan::CommandPool& commandPool;

		std::vector<Uniforms::Vertex> vertices;
		std::vector<uint32_t> indices;

		std::unique_ptr<class Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<class Vulkan::Buffer> indexBuffer;
		std::unique_ptr<class Vulkan::Image> image;
		std::unique_ptr<class Assets::TextureImage> textureImage;

		void CreateBuffers();
		void Load();
	};
}

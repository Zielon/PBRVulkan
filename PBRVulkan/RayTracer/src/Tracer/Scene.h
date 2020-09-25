#pragma once

#include <memory>


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

	private:
		const class Vulkan::Device& device;
		const class Vulkan::CommandPool& commandPool;
		std::unique_ptr<class Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<class Vulkan::Buffer> indexBuffer;
		std::unique_ptr<class Vulkan::Image> image;
		std::unique_ptr<class Assets::TextureImage> textureImage;

		void CreateBuffers();
	};
}

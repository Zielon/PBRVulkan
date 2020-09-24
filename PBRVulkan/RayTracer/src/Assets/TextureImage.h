#pragma once

#include "../Vulkan/Vulkan.h"

#include <string>
#include <memory>

namespace Vulkan
{
	class Buffer;
	class Device;
	class Image;
	class CommandBuffers;
}

namespace Assets
{
	class TextureImage
	{
	public:
		NON_COPIABLE(TextureImage)
		TextureImage(const Vulkan::Device& device,
		             const Vulkan::CommandBuffers& commandBuffers,
		             const std::string& path);
		~TextureImage() = default;

	private:
		std::unique_ptr<class Vulkan::Image> image;
	};
}

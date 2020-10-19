#pragma once

#include <memory>

#include "../Vulkan/Vulkan.h"
#include "../Vulkan/TextureSampler.h"
#include "../Vulkan/ImageView.h"
#include "../Vulkan/Image.h"

namespace Vulkan
{
	class Buffer;
	class Device;
	class TextureSampler;
	class CommandPool;
	class Image;
	class ImageView;
}

namespace Assets
{
	class Texture;
}

namespace Tracer
{
	class TextureImage
	{
	public:
		NON_COPIABLE(TextureImage)

		TextureImage(const Vulkan::Device& device,
		             const Vulkan::CommandPool& commandPool,
		             const Assets::Texture& texture,
		             VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
		             VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
		             VkImageType imageType = VK_IMAGE_TYPE_2D);
		~TextureImage() = default;

		[[nodiscard]] const Vulkan::Image& GetImage() const
		{
			return *image;
		}

		[[nodiscard]] VkImageView GetImageView() const
		{
			return imageView->Get();
		}

		[[nodiscard]] VkSampler GetTextureSampler() const
		{
			return sampler->Get();
		}

	private:
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> imageView;
		std::unique_ptr<Vulkan::TextureSampler> sampler;
	};
}

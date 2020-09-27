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
		             const Assets::Texture& texture);
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

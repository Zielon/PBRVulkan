#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Framebuffer final
	{
	public:
		Framebuffer(const class ImageView& imageView, const class SwapChain& swapChain, VkRenderPass renderPass);
		~Framebuffer();

		VkFramebuffer Get() const { return framebuffer; }

	private:
		const ImageView& imageView;
		VkFramebuffer framebuffer{};
	};
}

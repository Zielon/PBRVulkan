#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Framebuffer final
	{
	public:
		NON_COPIABLE(Framebuffer)

		Framebuffer(class ImageView& imageView, const class SwapChain& swapChain, VkRenderPass renderPass);
		~Framebuffer();

		[[nodiscard]] VkFramebuffer Get() const
		{
			return framebuffer;
		}

	private:
		const ImageView& imageView;
		VkFramebuffer framebuffer{};
	};
}

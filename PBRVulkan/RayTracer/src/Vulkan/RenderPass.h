#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	/*
	 * Specifies all the framebuffer attachments that will be used while rendering. It describes
	 * how many color and depth buffers there will be, how many samples to use for each of them and
	 * how their contents should be handled throughout the rendering operations.
	 */
	class RenderPass final
	{
	public:
		NON_COPIABLE(RenderPass)

		RenderPass(const class Device& device, const class SwapChain& swapChain, bool clearColorBuffer,
		           bool clearDepthBuffer);
		~RenderPass();

		[[nodiscard]] VkRenderPass Get() const
		{
			return renderPass;
		}

	private:
		VkRenderPass renderPass;
		const class Device& device;
	};
}

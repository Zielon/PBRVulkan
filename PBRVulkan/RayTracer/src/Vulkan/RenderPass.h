#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class RenderPass final
	{
	public:
		NON_COPIABLE(RenderPass)

		RenderPass(const class Device& device, const class SwapChain& swapChain);
		~RenderPass();
		
		[[nodiscard]] VkRenderPass Get() const { return renderPass; }
	private:
		VkRenderPass renderPass;
		const class Device& device;
	};
}

#pragma once

#include <memory>

#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;
	class SwapChain;
	class RenderPass;
	class CommandBuffers;
}

namespace Tracer
{
	class Menu
	{
	public:
		NON_COPIABLE(Menu)

		Menu(
			const Vulkan::Device& device,
			const Vulkan::SwapChain& swapChain,
			const Vulkan::CommandBuffers& commandBuffers);
		~Menu();

		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer) const;

	private:
		void RenderSettings() const;
		
		const Vulkan::SwapChain& swapChain;
		const Vulkan::Device& device;
		std::unique_ptr<Vulkan::RenderPass> renderPass;
	};
}

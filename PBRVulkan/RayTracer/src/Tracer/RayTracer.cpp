#include "RayTracer.h"

#include <array>

#include "Menu.h"

#include "../Vulkan/Device.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/SwapChain.h"

namespace Tracer
{
	RayTracer::RayTracer()
	{
		menu.reset(new Menu(*device, *swapChain, *commandBuffers));
	}

	void RayTracer::Render(VkFramebuffer framebuffer, VkCommandBuffer command)
	{
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = graphicsPipeline->GetRenderPass();
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain->Extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdDraw(command, 3, 1, 0, 0);
		}
		vkCmdEndRenderPass(command);

		//menu->Render(framebuffer, command);
	}

	RayTracer::~RayTracer() { }
}

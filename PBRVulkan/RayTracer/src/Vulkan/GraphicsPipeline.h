#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class GraphicsPipeline final
	{
	public:
		NON_COPIABLE(GraphicsPipeline)

		GraphicsPipeline(const class SwapChain& swapChain, const class Device& device);
		~GraphicsPipeline();

		VkPipelineLayout GetPipelineLayout() const { return pipelineLayout; }

		VkRenderPass GetRenderPass() const { return renderPass; }

		VkPipeline GetPipeline() const { return pipeline; }

	private:
		const SwapChain& swapChain;
		const Device& device;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		VkPipeline pipeline;
	};
}

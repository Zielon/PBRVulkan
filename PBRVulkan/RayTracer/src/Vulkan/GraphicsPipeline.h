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

		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const { return pipelineLayout; }
		[[nodiscard]] VkRenderPass GetRenderPass() const { return renderPass; }
		[[nodiscard]] VkPipeline GetPipeline() const { return pipeline; }

	private:
		void CreateRenderPass();
		void CreatePipeline();

		const SwapChain& swapChain;
		const Device& device;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		VkPipeline pipeline;
	};
}

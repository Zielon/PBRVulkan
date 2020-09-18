#pragma once

#include "Vulkan.h"
#include <memory>

namespace Vulkan
{
	class GraphicsPipeline final
	{
	public:
		NON_COPIABLE(GraphicsPipeline)

		GraphicsPipeline(const class SwapChain& swapChain, const class Device& device);
		~GraphicsPipeline();

		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const { return pipelineLayout; }
		[[nodiscard]] VkPipeline GetPipeline() const { return pipeline; }
		[[nodiscard]] VkRenderPass GetRenderPass() const;

	private:
		void CreateRenderPass();
		void CreatePipeline();

		const SwapChain& swapChain;
		const Device& device;
		std::unique_ptr<class RenderPass> renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
	};
}

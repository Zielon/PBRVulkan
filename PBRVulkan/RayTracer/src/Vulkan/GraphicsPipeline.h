#pragma once

#include "Vulkan.h"
#include <memory>

namespace Vulkan
{
	/*
	 *  The graphics pipeline is the sequence of operations that take the vertices and
	 *  textures of your meshes all the way to the pixels in the render targets.
	 */
	class GraphicsPipeline final
	{
	public:
		NON_COPIABLE(GraphicsPipeline)

		GraphicsPipeline(const class SwapChain& swapChain,
		                 const class Device& device,
		                 const class DescriptorSetLayout& descriptorSetLayout);
		~GraphicsPipeline();

		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const
		{
			return pipelineLayout;
		}

		[[nodiscard]] VkPipeline GetPipeline() const
		{
			return pipeline;
		}

		[[nodiscard]] VkRenderPass GetRenderPass() const;

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

	private:
		void CreatePipeline();

		const Device& device;
		const SwapChain& swapChain;
		const DescriptorSetLayout& descriptorSetLayout;
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<class RenderPass> renderPass;
	};
}

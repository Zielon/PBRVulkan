#pragma once

#include "Vulkan.h"
#include <memory>
#include <vector>

namespace Tracer
{
	class Scene;
}

namespace Vulkan
{
	/*
	 *  The graphics pipeline is the sequence of operations that take the vertices and
	 *  textures of your meshes all the way to the pixels in the render targets.
	 */
	class RasterizerGraphicsPipeline final
	{
	public:
		NON_COPIABLE(RasterizerGraphicsPipeline)

		RasterizerGraphicsPipeline(const class SwapChain& swapChain,
		                           const class Device& device,
		                           const Tracer::Scene& scene,
		                           const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers);
		~RasterizerGraphicsPipeline();

		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const
		{
			return pipelineLayout;
		}

		[[nodiscard]] VkPipeline GetPipeline() const
		{
			return pipeline;
		}

		[[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const
		{
			return descriptorSets;
		}

		[[nodiscard]] VkRenderPass GetRenderPass() const;

	private:
		const Device& device;
		const SwapChain& swapChain;

		VkPipeline pipeline{};
		VkPipelineLayout pipelineLayout{};

		std::vector<VkDescriptorSet> descriptorSets;
		std::unique_ptr<class RenderPass> renderPass;
		std::unique_ptr<class DescriptorsManager> descriptorsManager;
	};
}

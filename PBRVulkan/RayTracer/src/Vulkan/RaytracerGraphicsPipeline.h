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
	class RaytracerGraphicsPipeline final
	{
	public:
		NON_COPIABLE(RaytracerGraphicsPipeline)

		RaytracerGraphicsPipeline(const class SwapChain& swapChain,
		                          const class Device& device,
		                          const Tracer::Scene& scene,
		                          const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers);
		~RaytracerGraphicsPipeline();

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

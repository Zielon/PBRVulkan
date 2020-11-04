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
	 * By default the pipeline supports only 3 ray tracing shaders
	 * - ray generation
	 * - ray hit
	 * - ray miss
	 */
	class RaytracerGraphicsPipeline final
	{
	public:
		NON_COPIABLE(RaytracerGraphicsPipeline)

		RaytracerGraphicsPipeline(const class SwapChain& swapChain,
		                          const class Device& device,
		                          const Tracer::Scene& scene,
		                          const class ImageView& accumulationImage,
		                          const class ImageView& outputImage,
		                          const class ImageView& normalsImage,
		                          const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers,
		                          VkAccelerationStructureNV topLevelAS);
		~RaytracerGraphicsPipeline();

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

		[[nodiscard]] VkPipeline GetPipeline() const
		{
			return pipeline;
		}
		
		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const
		{
			return pipelineLayout;
		}
		
		[[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorsSets() const
		{
			return descriptorSets;
		}

	private:
		const Device& device;
		const SwapChain& swapChain;

		VkPipeline pipeline{};
		VkPipelineLayout pipelineLayout{};

		std::vector<VkDescriptorSet> descriptorSets;
		std::unique_ptr<class RenderPass> renderPass;
		std::unique_ptr<class Extensions> extensions;
		std::unique_ptr<class DescriptorsManager> descriptorsManager;
	};
}

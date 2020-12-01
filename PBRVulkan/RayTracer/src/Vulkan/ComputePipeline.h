#pragma once

#include "Vulkan.h"

#include <memory>
#include <vector>

namespace Vulkan
{
	class ComputePipeline final
	{
	public:
		NON_COPIABLE(ComputePipeline)

		ComputePipeline(const class SwapChain& swapChain,
		                const class Device& device,
		                const class ImageView& inputImage,
		                const class ImageView& outputImage,
		                const class ImageView& normalsImage,
		                const class ImageView& positionsImage,
		                const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers);

		~ComputePipeline();

		[[nodiscard]] const std::vector<VkPipeline>& GetComputePipelines() const
		{
			return pipelines;
		}

		[[nodiscard]] VkPipelineLayout GetPipelineLayout() const
		{
			return pipelineLayout;
		}

		[[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const
		{
			return descriptorSets;
		}

	private:
		const Device& device;
		const SwapChain& swapChain;

		std::vector<VkPipeline> pipelines{};
		VkPipelineLayout pipelineLayout{};

		std::vector<VkDescriptorSet> descriptorSets;
		std::unique_ptr<class DescriptorsManager> descriptorsManager;
	};
}

#pragma once

#include <memory>
#include <vector>

#include "Vulkan.h"

namespace Tracer
{
	class Scene;
}

namespace Vulkan
{
	class DescriptorsManager final
	{
	public:
		NON_COPIABLE(DescriptorsManager)

		DescriptorsManager(const class Device& device,
		                   const class SwapChain& swapChain,
		                   const class Tracer::Scene& scene,
		                   const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers);
		~DescriptorsManager();

		[[nodiscard]] VkDescriptorPool GetDescriptorPool() const
		{
			return descriptorPool;
		}

		[[nodiscard]] const class DescriptorSetLayout& GetDescriptorSetLayout() const
		{
			return *descriptorSetLayout;
		}

		[[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const
		{
			return descriptorSets;
		}

	private:
		const Device& device;
		const SwapChain& swapChain;
		VkDescriptorPool descriptorPool{};
		std::vector<VkDescriptorSet> descriptorSets;
		std::unique_ptr<class DescriptorSetLayout> descriptorSetLayout;

		void CreateDescriptorPool();
		void CreateDescriptorSets(const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers,
		                          const Tracer::Scene& scene);
	};
}

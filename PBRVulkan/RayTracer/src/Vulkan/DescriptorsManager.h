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
	/*
	 * Descriptor configuration used for:
	 *	VkDescriptorPoolSize
	 *	VkDescriptorBufferInfo
	 *	VkDescriptorImageInfo
	 */
	struct DescriptorBinding
	{
		uint32_t Binding;
		uint32_t DescriptorCount;
		VkDescriptorType Type;
		VkShaderStageFlags Stage;
	};

	class DescriptorsManager final
	{
	public:
		NON_COPIABLE(DescriptorsManager)

		DescriptorsManager(const class Device& device,
		                   const class SwapChain& swapChain,
		                   const std::vector<DescriptorBinding>& descriptorBindings);
		~DescriptorsManager();

		[[nodiscard]] VkDescriptorPool GetDescriptorPool() const
		{
			return descriptorPool;
		}

		[[nodiscard]] const class DescriptorSetLayout& GetDescriptorSetLayout() const
		{
			return *descriptorSetLayout;
		}

	private:
		const Device& device;
		const SwapChain& swapChain;
		VkDescriptorPool descriptorPool{};
		std::unique_ptr<class DescriptorSetLayout> descriptorSetLayout;

		void CreateDescriptorPool(const std::vector<DescriptorBinding>& descriptorBindings);
	};
}

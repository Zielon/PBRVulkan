#include "DescriptorsManager.h"

#include <array>

#include "../Tracer/Scene.h"

#include "Device.h"
#include "SwapChain.h"
#include "DescriptorSetLayout.h"

namespace Vulkan
{
	DescriptorsManager::DescriptorsManager(
		const Device& device,
		const SwapChain& swapChain,
		const std::vector<DescriptorBinding>& descriptorBindings):
		device(device), swapChain(swapChain)
	{
		descriptorSetLayout.reset(new DescriptorSetLayout(device, descriptorBindings));

		CreateDescriptorPool(descriptorBindings);
	}

	DescriptorsManager::~DescriptorsManager()
	{
		descriptorSetLayout.reset();

		if (descriptorPool != nullptr)
		{
			vkDestroyDescriptorPool(device.Get(), descriptorPool, nullptr);
			descriptorPool = nullptr;
		}
	}

	void DescriptorsManager::CreateDescriptorPool(const std::vector<DescriptorBinding>& descriptorBindings)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		poolSizes.reserve(descriptorBindings.size());
		for (const auto& binding : descriptorBindings)
			poolSizes.push_back(VkDescriptorPoolSize{ binding.Type, binding.DescriptorCount });

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swapChain.GetImage().size());

		VK_CHECK(vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &descriptorPool),
		         "Create descriptor pool");
	}
}

#include "DescriptorsManager.h"


#include "Buffer.h"
#include "Device.h"
#include "SwapChain.h"
#include "DescriptorSetLayout.h"
#include "../Geometry/MVP.h"

namespace Vulkan
{
	DescriptorsManager::DescriptorsManager(
		const Device& device,
		const SwapChain& swapChain,
		const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers):
		device(device), swapChain(swapChain)
	{
		descriptorSetLayout.reset(new DescriptorSetLayout(device));
		CreateDescriptorPool();
		CreateDescriptorSets(uniformBuffers);
	}

	DescriptorsManager::~DescriptorsManager()
	{
		if (descriptorPool != nullptr)
		{
			vkDestroyDescriptorPool(device.Get(), descriptorPool, nullptr);
			descriptorPool = nullptr;
		}
	}

	void DescriptorsManager::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(swapChain.GetImage().size());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(swapChain.GetImage().size());

		VK_CHECK(vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &descriptorPool),
		         "Create descriptor pool");
	}

	void DescriptorsManager::CreateDescriptorSets(const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers)
	{
		std::vector<VkDescriptorSetLayout> layouts(swapChain.GetImage().size(), descriptorSetLayout->Get());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.GetImage().size());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(swapChain.GetImage().size());

		VK_CHECK(vkAllocateDescriptorSets(device.Get(), &allocInfo, descriptorSets.data()),
		         "Allocate descriptor sets");

		for (size_t i = 0; i < swapChain.GetImage().size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i]->Get();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Uniforms::MVP);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device.Get(), 1, &descriptorWrite, 0, nullptr);
		}
	}
}

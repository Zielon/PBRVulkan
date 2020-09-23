#include "DescriptorSetLayout.h"

#include "Device.h"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const Device& device): device(device)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VK_CHECK(vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr, &descriptorSetLayout),
		         "Create descriptor set layout");
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (descriptorSetLayout != nullptr)
		{
			vkDestroyDescriptorSetLayout(device.Get(), descriptorSetLayout, nullptr);
			descriptorSetLayout = nullptr;
		}
	}
}

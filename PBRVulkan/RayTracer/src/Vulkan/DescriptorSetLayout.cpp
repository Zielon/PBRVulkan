#include "DescriptorSetLayout.h"

#include "Device.h"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const Device& device): device(device)
	{
		VkDescriptorSetLayoutBinding uniformLayoutBinding{};
		uniformLayoutBinding.binding = 0;
		uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformLayoutBinding.descriptorCount = 1;
		uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uniformLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uniformLayoutBinding;

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

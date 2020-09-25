#include "DescriptorSetLayout.h"

#include <array>


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

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uniformLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

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

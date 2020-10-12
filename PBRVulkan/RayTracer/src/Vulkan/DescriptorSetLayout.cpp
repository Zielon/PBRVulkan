#include "DescriptorSetLayout.h"

#include "Device.h"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const Device& device,
	                                         const std::vector<DescriptorBinding>& descriptorBindings): device(device)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (const auto& binding : descriptorBindings)
		{
			VkDescriptorSetLayoutBinding descriptor = {};
			descriptor.binding = binding.Binding;
			descriptor.descriptorCount = binding.DescriptorCount;
			descriptor.descriptorType = binding.Type;
			descriptor.stageFlags = binding.Stage;

			bindings.push_back(descriptor);
		}

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

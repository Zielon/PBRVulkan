#include "DescriptorSetLayout.h"

#include <array>

#include "../Tracer/Scene.h"
#include "Device.h"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const Device& device, const Tracer::Scene& scene): device(device)
	{
		// Uniforms layout
		VkDescriptorSetLayoutBinding uniformLayoutBinding{};
		uniformLayoutBinding.binding = 0;
		uniformLayoutBinding.descriptorCount = 1;
		uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uniformLayoutBinding.pImmutableSamplers = nullptr;

		// Materials layout
		VkDescriptorSetLayoutBinding materialLayoutBinding{};
		materialLayoutBinding.binding = 1;
		materialLayoutBinding.descriptorCount = 1;
		materialLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		materialLayoutBinding.pImmutableSamplers = nullptr;

		// Textures layout
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 2;
		samplerLayoutBinding.descriptorCount = static_cast<uint32_t>(scene.GetTextures().size());
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
			uniformLayoutBinding,
			materialLayoutBinding,
			samplerLayoutBinding
		};

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

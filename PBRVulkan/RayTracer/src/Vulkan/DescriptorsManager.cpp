#include "DescriptorsManager.h"

#include <array>

#include "../Geometry/MVP.h"

#include "../Tracer/Scene.h"
#include "../Tracer/TextureImage.h"

#include "Buffer.h"
#include "Device.h"
#include "SwapChain.h"
#include "DescriptorSetLayout.h"

namespace Vulkan
{
	DescriptorsManager::DescriptorsManager(
		const Device& device,
		const SwapChain& swapChain,
		const Tracer::Scene& scene,
		const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers):
		device(device), swapChain(swapChain)
	{
		descriptorSetLayout.reset(new DescriptorSetLayout(device, scene));
		CreateDescriptorPool(scene);
		CreateDescriptorSets(uniformBuffers, scene);
	}

	DescriptorsManager::~DescriptorsManager()
	{
		descriptorSets.clear();
		descriptorSetLayout.reset();

		if (descriptorPool != nullptr)
		{
			vkDestroyDescriptorPool(device.Get(), descriptorPool, nullptr);
			descriptorPool = nullptr;
		}
	}

	void DescriptorsManager::CreateDescriptorPool(const Tracer::Scene& scene)
	{
		std::array<VkDescriptorPoolSize, 3> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain.GetImage().size());

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain.GetImage().size());

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChain.GetImage().size() * scene.GetTextures().size());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swapChain.GetImage().size());

		VK_CHECK(vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &descriptorPool),
		         "Create descriptor pool");
	}

	void DescriptorsManager::CreateDescriptorSets(const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers,
	                                              const Tracer::Scene& scene)
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

		for (size_t imageIndex = 0; imageIndex < swapChain.GetImage().size(); imageIndex++)
		{
			std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

			// Uniforms descriptor
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[imageIndex]->Get();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Uniforms::MVP);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[imageIndex];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			// Material descriptor
			VkDescriptorBufferInfo materialInfo = {};
			materialInfo.buffer = scene.GetMaterialBuffer().Get();
			materialInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[imageIndex];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &materialInfo;

			// Texture descriptor
			std::vector<VkDescriptorImageInfo> imageInfos(scene.GetTextures().size());

			for (size_t t = 0; t < imageInfos.size(); ++t)
			{
				imageInfos[t].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos[t].imageView = scene.GetTextures()[t]->GetImageView();
				imageInfos[t].sampler = scene.GetTextures()[t]->GetTextureSampler();
			}

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = descriptorSets[imageIndex];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[2].descriptorCount = static_cast<uint32_t>(imageInfos.size());
			descriptorWrites[2].pImageInfo = imageInfos.data();

			vkUpdateDescriptorSets(device.Get(), static_cast<uint32_t>(descriptorWrites.size()),
			                       descriptorWrites.data(), 0, nullptr);
		}
	}
}

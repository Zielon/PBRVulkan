#include "RaytracerGraphicsPipeline.h"

#include "Extensions.h"
#include "SwapChain.h"
#include "Device.h"
#include "Shader.h"
#include "RenderPass.h"
#include "DescriptorSetLayout.h"
#include "DescriptorsManager.h"
#include "Buffer.h"

#include "../Tracer/Scene.h"
#include "../Tracer/TextureImage.h"

namespace Vulkan
{
	RaytracerGraphicsPipeline::RaytracerGraphicsPipeline(
		const SwapChain& swapChain,
		const Device& device,
		const Tracer::Scene& scene,
		const ImageView& accumulationImage,
		const ImageView& outputImage,
		const ImageView& normalsImage,
		const ImageView& positionsImage,
		const std::vector<std::unique_ptr<Buffer>>& uniformBuffers,
		VkAccelerationStructureKHR topLevelAS):
		device(device),
		swapChain(swapChain),
		renderPass(new RenderPass(device, swapChain, true, true))
	{
		extensions.reset(new Extensions(device));

		// Load shaders.
		const Shader rayGenShader(device, "Raytracing.compiled.rgen.spv");
		const Shader missShader(device, "Raytracing.compiled.rmiss.spv");
		const Shader shadowShader(device, "Shadow.compiled.rmiss.spv");
		const Shader closestHitShader(device, "Raytracing.compiled.rchit.spv");

		std::array<VkPipelineShaderStageCreateInfo, 4> shaderStages =
		{
			rayGenShader.CreateShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
			missShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
			shadowShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
			closestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
		};

		// Shader group
		VkRayTracingShaderGroupCreateInfoKHR rayGenGroupInfo = {};
		rayGenGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		rayGenGroupInfo.pNext = nullptr;
		rayGenGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		rayGenGroupInfo.generalShader = 0;
		rayGenGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;

		VkRayTracingShaderGroupCreateInfoKHR missGroupInfo = {};
		missGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		missGroupInfo.pNext = nullptr;
		missGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		missGroupInfo.generalShader = 1;
		missGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
		missGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
		missGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;

		VkRayTracingShaderGroupCreateInfoKHR shadowGroupInfo = {};
		shadowGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		shadowGroupInfo.pNext = nullptr;
		shadowGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		shadowGroupInfo.generalShader = 2;
		shadowGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
		shadowGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
		shadowGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;

		VkRayTracingShaderGroupCreateInfoKHR triangleHitGroupInfo = {};
		triangleHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		triangleHitGroupInfo.pNext = nullptr;
		triangleHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		triangleHitGroupInfo.generalShader = VK_SHADER_UNUSED_KHR;
		triangleHitGroupInfo.closestHitShader = 3;
		triangleHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
		triangleHitGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;

		std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups =
		{
			rayGenGroupInfo,
			missGroupInfo,
			shadowGroupInfo,
			triangleHitGroupInfo,
		};

		std::vector<DescriptorBinding> descriptorBindings =
		{
			// Top level acceleration structure.
			{
				0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
				VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
			},

			// Image accumulation
			{ 1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR },

			// Output
			{ 2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR },

			// Uniforms
			{
				3, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
			},

			// Vertex buffer
			{
				4, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR
			},

			// Index buffer
			{ 5, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },

			// Material buffer
			{ 6, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },

			// Offset buffer
			{ 7, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },

			// Textures
			{
				8, scene.GetTextureSize(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR
			},

			// Lights buffer
			{
				9, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR
			},

			// Normal
			{
				10, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR
			},

			// World position
			{
				11, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR
			}
		};

		// HDRs
		if (scene.UseHDR())
		{
			descriptorBindings.push_back(
				{
					12, static_cast<uint32_t>(scene.GetHDRTextures().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR
				});
		}

		descriptorsManager.reset(new DescriptorsManager(device, swapChain, descriptorBindings));

		std::vector<VkDescriptorSetLayout> layouts(swapChain.GetImage().size(),
		                                           descriptorsManager->GetDescriptorSetLayout().Get());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorsManager->GetDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.GetImage().size());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(swapChain.GetImage().size());

		VK_CHECK(vkAllocateDescriptorSets(device.Get(), &allocInfo, descriptorSets.data()),
		         "Allocate descriptor sets");

		for (size_t imageIndex = 0; imageIndex < swapChain.GetImage().size(); imageIndex++)
		{
			std::vector<VkWriteDescriptorSet> descriptorWrites(12);

			// Top level acceleration structure.
			VkWriteDescriptorSetAccelerationStructureKHR structureInfo = {};
			structureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			structureInfo.pNext = nullptr;
			structureInfo.accelerationStructureCount = 1;
			structureInfo.pAccelerationStructures = &topLevelAS;

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[imageIndex];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pNext = &structureInfo;

			// Accumulation image
			VkDescriptorImageInfo accumulationImageInfo = {};
			accumulationImageInfo.imageView = accumulationImage.Get();
			accumulationImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[imageIndex];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &accumulationImageInfo;

			// Output image
			VkDescriptorImageInfo outputImageInfo = {};
			outputImageInfo.imageView = outputImage.Get();
			outputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = descriptorSets[imageIndex];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &outputImageInfo;

			// Uniform buffer
			VkDescriptorBufferInfo uniformBufferInfo = {};
			uniformBufferInfo.buffer = uniformBuffers[imageIndex]->Get();
			uniformBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = descriptorSets[imageIndex];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pBufferInfo = &uniformBufferInfo;

			// Vertex buffer
			VkDescriptorBufferInfo vertexBufferInfo = {};
			vertexBufferInfo.buffer = scene.GetVertexBuffer().Get();
			vertexBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[4].dstSet = descriptorSets[imageIndex];
			descriptorWrites[4].dstBinding = 4;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pBufferInfo = &vertexBufferInfo;

			// Index buffer
			VkDescriptorBufferInfo indexBufferInfo = {};
			indexBufferInfo.buffer = scene.GetIndexBuffer().Get();
			indexBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[5].dstSet = descriptorSets[imageIndex];
			descriptorWrites[5].dstBinding = 5;
			descriptorWrites[5].dstArrayElement = 0;
			descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[5].descriptorCount = 1;
			descriptorWrites[5].pBufferInfo = &indexBufferInfo;

			// Material buffer
			VkDescriptorBufferInfo materialBufferInfo = {};
			materialBufferInfo.buffer = scene.GetMaterialBuffer().Get();
			materialBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[6].dstSet = descriptorSets[imageIndex];
			descriptorWrites[6].dstBinding = 6;
			descriptorWrites[6].dstArrayElement = 0;
			descriptorWrites[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[6].descriptorCount = 1;
			descriptorWrites[6].pBufferInfo = &materialBufferInfo;

			// Offsets buffer
			VkDescriptorBufferInfo offsetsBufferInfo = {};
			offsetsBufferInfo.buffer = scene.GetOffsetBuffer().Get();
			offsetsBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[7].dstSet = descriptorSets[imageIndex];
			descriptorWrites[7].dstBinding = 7;
			descriptorWrites[7].dstArrayElement = 0;
			descriptorWrites[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[7].descriptorCount = 1;
			descriptorWrites[7].pBufferInfo = &offsetsBufferInfo;

			// Texture descriptor
			std::vector<VkDescriptorImageInfo> imageInfos(scene.GetTextures().size());

			for (size_t t = 0; t < imageInfos.size(); ++t)
			{
				imageInfos[t].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos[t].imageView = scene.GetTextures()[t]->GetImageView();
				imageInfos[t].sampler = scene.GetTextures()[t]->GetTextureSampler();
			}

			descriptorWrites[8].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[8].dstSet = descriptorSets[imageIndex];
			descriptorWrites[8].dstBinding = 8;
			descriptorWrites[8].dstArrayElement = 0;
			descriptorWrites[8].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[8].descriptorCount = static_cast<uint32_t>(imageInfos.size());
			descriptorWrites[8].pImageInfo = imageInfos.data();

			// Lights buffer
			VkDescriptorBufferInfo lightsBufferInfo = {};
			lightsBufferInfo.buffer = scene.GetLightsBuffer().Get();
			lightsBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites[9].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[9].dstSet = descriptorSets[imageIndex];
			descriptorWrites[9].dstBinding = 9;
			descriptorWrites[9].dstArrayElement = 0;
			descriptorWrites[9].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[9].descriptorCount = 1;
			descriptorWrites[9].pBufferInfo = &lightsBufferInfo;

			// Normal image
			VkDescriptorImageInfo normalsImageInfo = {};
			normalsImageInfo.imageView = normalsImage.Get();
			normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			descriptorWrites[10].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[10].dstSet = descriptorSets[imageIndex];
			descriptorWrites[10].dstBinding = 10;
			descriptorWrites[10].dstArrayElement = 0;
			descriptorWrites[10].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[10].descriptorCount = 1;
			descriptorWrites[10].pImageInfo = &normalsImageInfo;

			// Position image
			VkDescriptorImageInfo positionImageInfo = {};
			positionImageInfo.imageView = positionsImage.Get();
			positionImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			descriptorWrites[11].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[11].dstSet = descriptorSets[imageIndex];
			descriptorWrites[11].dstBinding = 11;
			descriptorWrites[11].dstArrayElement = 0;
			descriptorWrites[11].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[11].descriptorCount = 1;
			descriptorWrites[11].pImageInfo = &positionImageInfo;

			// Outside the block because of RAII 
			std::vector<VkDescriptorImageInfo> hdrInfos(scene.GetHDRTextures().size());
			VkWriteDescriptorSet descriptor{};

			// HDR descriptor
			if (scene.UseHDR())
			{
				for (size_t t = 0; t < hdrInfos.size(); ++t)
				{
					hdrInfos[t].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					hdrInfos[t].imageView = scene.GetHDRTextures()[t]->GetImageView();
					hdrInfos[t].sampler = scene.GetHDRTextures()[t]->GetTextureSampler();
				}

				descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor.dstSet = descriptorSets[imageIndex];
				descriptor.dstBinding = 12;
				descriptor.dstArrayElement = 0;
				descriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor.descriptorCount = static_cast<uint32_t>(hdrInfos.size());
				descriptor.pImageInfo = hdrInfos.data();

				descriptorWrites.push_back(descriptor);
			}

			vkUpdateDescriptorSets(device.Get(), static_cast<uint32_t>(descriptorWrites.size()),
			                       descriptorWrites.data(), 0, nullptr);
		}

		VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorsManager->GetDescriptorSetLayout().Get() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;

		VK_CHECK(vkCreatePipelineLayout(device.Get(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
		         "Create ray tracing graphics pipeline layout");

		// Create graphic pipeline
		VkRayTracingPipelineCreateInfoKHR pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.flags = 0;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.groupCount = static_cast<uint32_t>(groups.size());
		pipelineInfo.pGroups = groups.data();
		pipelineInfo.maxPipelineRayRecursionDepth = 2;
		pipelineInfo.layout = pipelineLayout;

		VK_CHECK(extensions->vkCreateRayTracingPipelinesKHR(device.Get(), nullptr, nullptr, 1, &pipelineInfo, nullptr, &pipeline),
		         "Create ray tracing pipeline");
	}

	RaytracerGraphicsPipeline::~RaytracerGraphicsPipeline()
	{
		if (pipeline != nullptr)
		{
			vkDestroyPipeline(device.Get(), pipeline, nullptr);
			pipeline = nullptr;
		}

		renderPass.reset();

		if (pipelineLayout != nullptr)
		{
			vkDestroyPipelineLayout(device.Get(), pipelineLayout, nullptr);
			pipelineLayout = nullptr;
		}

		descriptorSets.clear();
	}
}

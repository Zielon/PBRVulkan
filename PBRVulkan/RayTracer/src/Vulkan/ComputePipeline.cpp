#include "ComputePipeline.h"

#include <array>

#include "RenderPass.h"
#include "Shader.h"
#include "SwapChain.h"
#include "Device.h"
#include "Buffer.h"
#include "ImageView.h"
#include "DescriptorSetLayout.h"
#include "DescriptorsManager.h"

#include "../Geometry/Compute.h"

namespace Vulkan
{
	ComputePipeline::ComputePipeline(const SwapChain& swapChain,
	                                 const Device& device,
	                                 const ImageView& inputImage,
	                                 const ImageView& outputImage,
	                                 const ImageView& normalsImage,
	                                 const ImageView& positionsImage,
	                                 const std::vector<std::unique_ptr<class Buffer>>& uniformBuffers)
		: device(device), swapChain(swapChain)
	{
		const Shader denoiserShader(device, "Denoiser.comp.spv");
		const Shader edgeDetectShader(device, "Edgedetect.comp.spv");
		const Shader sharpenShader(device, "Sharpen.comp.spv");

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
		{
			denoiserShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT),
			edgeDetectShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT),
			sharpenShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT)
		};

		const std::vector<DescriptorBinding> descriptorBindings =
		{
			{ 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT },
			{ 1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT },
			{ 2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT },
			{ 3, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT },
			{ 4, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT },
		};

		descriptorsManager.reset(new DescriptorsManager(device, swapChain, descriptorBindings));

		std::vector<VkDescriptorSetLayout> layouts(1, descriptorsManager->GetDescriptorSetLayout().Get());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorsManager->GetDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(1);

		VK_CHECK(vkAllocateDescriptorSets(device.Get(), &allocInfo, descriptorSets.data()),
		         "Allocate descriptor sets");

		std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

		// Input image
		VkDescriptorImageInfo inputImageInfo = {};
		inputImageInfo.imageView = inputImage.Get();
		inputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[0];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &inputImageInfo;

		// Output image
		VkDescriptorImageInfo outputImageInfo = {};
		outputImageInfo.imageView = outputImage.Get();
		outputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[0];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &outputImageInfo;

		// Normals image
		VkDescriptorImageInfo normalsImageInfo = {};
		normalsImageInfo.imageView = normalsImage.Get();
		normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSets[0];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &normalsImageInfo;

		// Positions image
		VkDescriptorImageInfo positionsImageInfo = {};
		positionsImageInfo.imageView = positionsImage.Get();
		positionsImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = descriptorSets[0];
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &positionsImageInfo;

		// Uniforms descriptor
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[0]->Get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(Uniforms::Compute);

		descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[4].dstSet = descriptorSets[0];
		descriptorWrites[4].dstBinding = 4;
		descriptorWrites[4].dstArrayElement = 0;
		descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[4].descriptorCount = 1;
		descriptorWrites[4].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device.Get(), static_cast<uint32_t>(descriptorWrites.size()),
		                       descriptorWrites.data(), 0, nullptr);


		VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorsManager->GetDescriptorSetLayout().Get() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;

		VK_CHECK(vkCreatePipelineLayout(device.Get(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
		         "Create compute pipeline layout");

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		for (auto& shader : shaderStages)
		{
			VkPipeline pipeline;
			pipelineInfo.stage = shader;
			VK_CHECK(vkCreateComputePipelines(device.Get(), nullptr, 1, &pipelineInfo, nullptr, &pipeline),
			         "Create compute pipeline");
			pipelines.push_back(pipeline);
		}
	}

	ComputePipeline::~ComputePipeline()
	{
		for (auto* pipeline : pipelines)
			vkDestroyPipeline(device.Get(), pipeline, nullptr);

		if (pipelineLayout != nullptr)
		{
			vkDestroyPipelineLayout(device.Get(), pipelineLayout, nullptr);
			pipelineLayout = nullptr;
		}
	}
}

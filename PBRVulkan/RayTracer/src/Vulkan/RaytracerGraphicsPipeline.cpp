#include "RaytracerGraphicsPipeline.h"

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
		const std::vector<std::unique_ptr<Buffer>>& uniformBuffers):
		device(device),
		swapChain(swapChain),
		renderPass(new RenderPass(device, swapChain, true, true))
	{
		// Load shaders.
		const Shader rayGenShader(device, "Raytracing.rgen.spv");
		const Shader missShader(device, "Raytracing.rmiss.spv");
		const Shader closestHitShader(device, "Raytracing.rchit.spv");

		VkPipelineShaderStageCreateInfo shaderStages[] =
		{
			rayGenShader.CreateShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_NV),
			missShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_NV),
			closestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
		};

		// Shader group
		VkRayTracingShaderGroupCreateInfoNV rayGenGroupInfo = {};
		rayGenGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		rayGenGroupInfo.pNext = nullptr;
		rayGenGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
		rayGenGroupInfo.generalShader = 0;
		rayGenGroupInfo.closestHitShader = VK_SHADER_UNUSED_NV;
		rayGenGroupInfo.anyHitShader = VK_SHADER_UNUSED_NV;
		rayGenGroupInfo.intersectionShader = VK_SHADER_UNUSED_NV;
		rayGenIndex = 0;

		VkRayTracingShaderGroupCreateInfoNV missGroupInfo = {};
		missGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		missGroupInfo.pNext = nullptr;
		missGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
		missGroupInfo.generalShader = 1;
		missGroupInfo.closestHitShader = VK_SHADER_UNUSED_NV;
		missGroupInfo.anyHitShader = VK_SHADER_UNUSED_NV;
		missGroupInfo.intersectionShader = VK_SHADER_UNUSED_NV;
		missIndex = 1;

		VkRayTracingShaderGroupCreateInfoNV triangleHitGroupInfo = {};
		triangleHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		triangleHitGroupInfo.pNext = nullptr;
		triangleHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV;
		triangleHitGroupInfo.generalShader = VK_SHADER_UNUSED_NV;
		triangleHitGroupInfo.closestHitShader = 2;
		triangleHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_NV;
		triangleHitGroupInfo.intersectionShader = VK_SHADER_UNUSED_NV;
		triangleHitGroupIndex = 2;
	}

	RaytracerGraphicsPipeline::~RaytracerGraphicsPipeline() { }
}

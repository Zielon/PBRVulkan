#include "ShaderBindingTable.h"

#include "Vulkan.h"

#include "Buffer.h"
#include "Device.h"
#include "Extensions.h"
#include "RaytracerGraphicsPipeline.h"

namespace Vulkan
{
	namespace SBT
	{
		size_t RoundUp(size_t size, size_t powerOf2Alignment)
		{
			return (size + powerOf2Alignment - 1) & ~(powerOf2Alignment - 1);
		}
	}

	ShaderBindingTable::ShaderBindingTable(const RaytracerGraphicsPipeline& raytracerPipeline)
		: raytracerPipeline(raytracerPipeline)
	{
		VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationProperties{};
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR pipelineRTProperties{};

		accelerationProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		pipelineRTProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		pipelineRTProperties.pNext = &accelerationProperties;

		VkPhysicalDeviceProperties2 properties = {};
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		properties.pNext = &pipelineRTProperties;
		vkGetPhysicalDeviceProperties2(raytracerPipeline.GetDevice().GetPhysical(), &properties);

		extensions.reset(new Extensions(raytracerPipeline.GetDevice()));

		uint32_t handleSize = pipelineRTProperties.shaderGroupHandleSize;
		uint32_t groupCount = 4;
		entrySize = SBT::RoundUp(handleSize, pipelineRTProperties.shaderGroupBaseAlignment);

		const size_t stbSize = groupCount * entrySize;

		const auto usage = static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		stbBuffer.reset(new Buffer(raytracerPipeline.GetDevice(), stbSize, usage,
		                           VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
		                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

		std::vector<uint8_t> shaderHandleStorage(groupCount * handleSize);

		VK_CHECK(extensions->vkGetRayTracingShaderGroupHandlesKHR(
			         raytracerPipeline.GetDevice().Get(),
			         raytracerPipeline.GetPipeline(), static_cast<uint32_t>(0), groupCount,
			         shaderHandleStorage.size(),
			         shaderHandleStorage.data()),
		         "Get ray tracing shader group handles");

		// =============== COPY SHADER GROUP ===============

		auto* dst = static_cast<uint8_t*>(stbBuffer->Map(0, stbSize));

		// Gen
		std::memcpy(dst, shaderHandleStorage.data() + 0 * handleSize, handleSize);
		dst += entrySize;

		// Miss
		std::memcpy(dst, shaderHandleStorage.data() + 1 * handleSize, handleSize);
		dst += entrySize;
		std::memcpy(dst, shaderHandleStorage.data() + 2 * handleSize, handleSize);
		dst += entrySize;

		// Hit
		std::memcpy(dst, shaderHandleStorage.data() + 3 * handleSize, handleSize);

		stbBuffer->Unmap();
	}

	ShaderBindingTable::~ShaderBindingTable() { }
}

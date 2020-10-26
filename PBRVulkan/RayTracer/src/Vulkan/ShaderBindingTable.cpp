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
		VkPhysicalDeviceRayTracingPropertiesNV propertiesNV{};
		propertiesNV.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

		VkPhysicalDeviceProperties2 properties = {};
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		properties.pNext = &propertiesNV;
		vkGetPhysicalDeviceProperties2(raytracerPipeline.GetDevice().GetPhysical(), &properties);

		extensions.reset(new Extensions(raytracerPipeline.GetDevice()));

		uint32_t handleSize = propertiesNV.shaderGroupHandleSize;
		uint32_t groupCount = 4;
		entrySize = SBT::RoundUp(handleSize, propertiesNV.shaderGroupBaseAlignment);

		const size_t stbSize = groupCount * entrySize;

		stbBuffer.reset(new Buffer(raytracerPipeline.GetDevice(), stbSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

		std::vector<uint8_t> shaderHandleStorage(groupCount * handleSize);

		VK_CHECK(extensions->vkGetRayTracingShaderGroupHandlesNV(
			         raytracerPipeline.GetDevice().Get(),
			         raytracerPipeline.GetPipeline(), uint32_t(0), groupCount,
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

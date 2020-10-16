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

		entrySize = SBT::RoundUp(handleSize, propertiesNV.shaderGroupBaseAlignment);

		const size_t stbSize = 3 * entrySize;

		stbBuffer.reset(new Buffer(raytracerPipeline.GetDevice(), stbSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

		std::vector<uint8_t> shaderHandleStorage(3 * handleSize);

		VK_CHECK(extensions->vkGetRayTracingShaderGroupHandlesNV(
			         raytracerPipeline.GetDevice().Get(),
			         raytracerPipeline.GetPipeline(), uint32_t(0), uint32_t(3),
			         shaderHandleStorage.size(),
			         shaderHandleStorage.data()),
		         "Get ray tracing shader group handles");

		// =============== COPY SHADER GROUP ===============

		auto* dst = static_cast<uint8_t*>(stbBuffer->Map(0, stbSize));

		std::memcpy(
			dst, shaderHandleStorage.data() + RaytracerGraphicsPipeline::GetRayGenShaderIndex() * handleSize, handleSize);
		dst += entrySize;
		std::memcpy(
			dst, shaderHandleStorage.data() + RaytracerGraphicsPipeline::GetMissShaderIndex() * handleSize, handleSize);
		dst += entrySize;
		std::memcpy(
			dst, shaderHandleStorage.data() + RaytracerGraphicsPipeline::GetHitShaderIndex() * handleSize, handleSize);

		stbBuffer->Unmap();
	}

	ShaderBindingTable::~ShaderBindingTable() { }
}

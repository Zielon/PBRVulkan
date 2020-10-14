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

		size_t CopyShaderData(
			uint8_t* const dst,
			uint32_t shaderGroupHandleSize,
			uint32_t groupIndex,
			const size_t entrySize,
			const uint8_t* const shaderHandleStorage)
		{
			uint8_t* pDst = dst;

			// Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesNV.
			std::memcpy(pDst, shaderHandleStorage + groupIndex * shaderGroupHandleSize, shaderGroupHandleSize);

			return entrySize;
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
		stbBuffer.reset(new Buffer(raytracerPipeline.GetDevice(), 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

		const auto entrySize = SBT::RoundUp(propertiesNV.shaderGroupHandleSize,
		                                    propertiesNV.shaderGroupBaseAlignment);
		const size_t stbSize = 3 * entrySize;

		std::vector<uint8_t> shaderHandleStorage(uint32_t(3) * propertiesNV.shaderGroupHandleSize);

		VK_CHECK(extensions->vkGetRayTracingShaderGroupHandlesNV(
			         raytracerPipeline.GetDevice().Get(),
			         raytracerPipeline.GetPipeline(), uint32_t(0), uint32_t(3),
			         shaderHandleStorage.size(),
			         shaderHandleStorage.data()),
		         "Get ray tracing shader group handles");

		auto* data = static_cast<uint8_t*>(stbBuffer->Map(0, 3));

		data += SBT::CopyShaderData(data, propertiesNV.shaderGroupHandleSize,
		                            RaytracerGraphicsPipeline::GetRayGenShaderIndex(), entrySize,
		                            shaderHandleStorage.data());

		data += SBT::CopyShaderData(data, propertiesNV.shaderGroupHandleSize,
		                            RaytracerGraphicsPipeline::GetMissShaderIndex(), entrySize,
		                            shaderHandleStorage.data());

		SBT::CopyShaderData(data, propertiesNV.shaderGroupHandleSize, RaytracerGraphicsPipeline::GetHitShaderIndex(),
		                    entrySize, shaderHandleStorage.data());

		stbBuffer->Unmap();
	}

	ShaderBindingTable::~ShaderBindingTable()
	{
		extensions.reset();
		stbBuffer.reset();
	}
}

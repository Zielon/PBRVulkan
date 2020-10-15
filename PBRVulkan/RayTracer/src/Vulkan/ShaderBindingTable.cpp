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
			const uint8_t* const src)
		{
			uint8_t* pDst = dst;

			std::vector<unsigned char> inlineData;

			// Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesNV.
			std::memcpy(pDst, src + groupIndex * shaderGroupHandleSize, shaderGroupHandleSize);

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

		auto genRayIndex = RaytracerGraphicsPipeline::GetRayGenShaderIndex();
		auto missRayIndex = RaytracerGraphicsPipeline::GetMissShaderIndex();
		auto hitRayIndex = RaytracerGraphicsPipeline::GetHitShaderIndex();

		dst += SBT::CopyShaderData(dst, handleSize, genRayIndex, entrySize, shaderHandleStorage.data());
		dst += SBT::CopyShaderData(dst, handleSize, missRayIndex, entrySize, shaderHandleStorage.data());
		SBT::CopyShaderData(dst, handleSize, hitRayIndex, entrySize, shaderHandleStorage.data());

		stbBuffer->Unmap();
	}

	ShaderBindingTable::~ShaderBindingTable()
	{
		extensions.reset();
		stbBuffer.reset();
	}
}

#include "BLAS.h"

#include <vector>

#include "Device.h"
#include "Buffer.h"
#include "Memory.h"
#include "Extensions.h"

#include "../Tracer/Scene.h"
#include "../Geometry/Vertex.h"

namespace Vulkan
{
	void BLAS::Generate(
		VkCommandBuffer commandBuffer,
		const Buffer& scratchBuffer,
		VkDeviceSize scratchOffset,
		const Buffer& blasBuffer,
		VkDeviceSize resultOffset)
	{
		Create(blasBuffer, resultOffset);

		const VkAccelerationStructureBuildRangeInfoKHR* buildOffsetInfo = buildOffsets.data();

		buildGeometryInfo.dstAccelerationStructure = accelerationStructure;
		buildGeometryInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;
		extensions->vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildGeometryInfo, &buildOffsetInfo);
	}

	VkAccelerationStructureGeometryKHR BLAS::CreateGeometry(
		const Tracer::Scene& scene,
		uint32_t vertexOffset,
		uint32_t vertexCount,
		uint32_t indexOffset,
		uint32_t indexCount,
		bool isOpaque)
	{
		VkAccelerationStructureGeometryKHR geometry = {};

		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.pNext = nullptr;
		geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		geometry.geometry.triangles.pNext = nullptr;
		geometry.geometry.triangles.vertexData.deviceAddress = scene.GetVertexBuffer().GetDeviceAddress();
		geometry.geometry.triangles.vertexStride = sizeof(Geometry::Vertex);
		geometry.geometry.triangles.maxVertex = vertexCount;
		geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		geometry.geometry.triangles.indexData.deviceAddress = scene.GetIndexBuffer().GetDeviceAddress();
		geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		geometry.geometry.triangles.transformData = {};
		geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

		VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};

		buildOffsetInfo.firstVertex = vertexOffset / sizeof(Geometry::Vertex);
		buildOffsetInfo.primitiveOffset = indexOffset;
		buildOffsetInfo.primitiveCount = indexCount / 3;
		buildOffsetInfo.transformOffset = 0;
	}
}

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
	namespace BLASHelper
	{
		VkAccelerationStructureCreateInfoNV GetCreateInfo(const std::vector<VkGeometryNV>& geometries,
		                                                  const bool allowUpdate)
		{
			const auto flags = allowUpdate
				                   ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV
				                   : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

			VkAccelerationStructureCreateInfoNV structureInfo = {};
			structureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
			structureInfo.pNext = nullptr;
			structureInfo.compactedSize = 0;
			structureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
			structureInfo.info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
			structureInfo.info.flags = flags;
			structureInfo.info.instanceCount = 0;
			// The bottom-level AS can only contain explicit geometry, and no instances
			structureInfo.info.geometryCount = static_cast<uint32_t>(geometries.size());
			structureInfo.info.pGeometries = geometries.data();

			return structureInfo;
		}
	}

	BLAS::BLAS(const class Device& device, const std::vector<VkGeometryNV>& geometries, bool allowUpdate) :
		AccelerationStructure(device, BLASHelper::GetCreateInfo(geometries, allowUpdate)), geometries(geometries) {}

	BLAS::BLAS(BLAS&& other) noexcept :
		AccelerationStructure(std::move(other)),
		geometries(std::move(other.geometries)) {}

	void BLAS::Generate(
		VkCommandBuffer commandBuffer,
		const Buffer& scratchBuffer,
		VkDeviceSize scratchOffset,
		const Buffer& blasBuffer,
		VkDeviceSize resultOffset,
		bool updateOnly) const
	{
		if (updateOnly && !allowUpdate)
		{
			throw std::invalid_argument("Cannot update readonly structure!");
		}

		const VkAccelerationStructureNV previousStructure = updateOnly ? accelerationStructure : nullptr;

		// Bind the acceleration structure descriptor to the actual memory that will contain it
		VkBindAccelerationStructureMemoryInfoNV bindInfo = {};
		bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bindInfo.pNext = nullptr;
		bindInfo.accelerationStructure = accelerationStructure;
		bindInfo.memory = blasBuffer.GetMemory().Get();
		bindInfo.memoryOffset = resultOffset;
		bindInfo.deviceIndexCount = 0;
		bindInfo.pDeviceIndices = nullptr;

		VK_CHECK(extensions->vkBindAccelerationStructureMemoryNV(device.Get(), 1, &bindInfo),
		         "Bind acceleration structure");

		const auto flags = allowUpdate
			                   ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV
			                   : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

		VkAccelerationStructureInfoNV buildInfo = {};
		buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		buildInfo.pNext = nullptr;
		buildInfo.flags = flags;
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
		buildInfo.instanceCount = 0;
		buildInfo.geometryCount = static_cast<uint32_t>(geometries.size());
		buildInfo.pGeometries = geometries.data();

		extensions->vkCmdBuildAccelerationStructureNV(
			commandBuffer,
			&buildInfo,
			nullptr,
			0,
			updateOnly,
			accelerationStructure,
			previousStructure,
			scratchBuffer.Get(),
			scratchOffset);
	}

	VkGeometryNV BLAS::CreateGeometry(
		const Tracer::Scene& scene,
		uint32_t vertexOffset,
		uint32_t vertexCount,
		uint32_t indexOffset,
		uint32_t indexCount,
		bool isOpaque)
	{
		VkGeometryNV geometry = {};
		geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
		geometry.pNext = nullptr;
		geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
		geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
		geometry.geometry.triangles.pNext = nullptr;
		geometry.geometry.triangles.vertexData = scene.GetVertexBuffer().Get();
		geometry.geometry.triangles.vertexOffset = vertexOffset;
		geometry.geometry.triangles.vertexCount = vertexCount;
		geometry.geometry.triangles.vertexStride = sizeof(Geometry::Vertex);
		geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		geometry.geometry.triangles.indexData = scene.GetIndexBuffer().Get();
		geometry.geometry.triangles.indexOffset = indexOffset;
		geometry.geometry.triangles.indexCount = indexCount;
		geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		geometry.geometry.triangles.transformData = nullptr;
		geometry.geometry.triangles.transformOffset = 0;
		geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
		geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_NV : 0;

		return geometry;
	}
}

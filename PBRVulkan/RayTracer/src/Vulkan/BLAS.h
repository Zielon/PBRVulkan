#pragma once

#include "AccelerationStructure.h"

#include <vector>

namespace Tracer
{
	class Scene;
}

namespace Vulkan
{
	/**
	 * Bottom Level Acceleration Structure geometry wrapper
	 */
	class BLASGeometry
	{
	public:
		void CreateGeometry(
			const Tracer::Scene& scene,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			uint32_t indexOffset,
			uint32_t indexCount,
			bool isOpaque);
		
		std::vector<VkAccelerationStructureGeometryKHR> triangles;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildOffsets;
	};
	
	/**
	 * Bottom Level Acceleration Structure
	 */
	class BLAS final : public AccelerationStructure
	{
	public:
		BLAS(const BLAS&) = delete;
		BLAS& operator = (const BLAS&) = delete;
		BLAS& operator = (BLAS&&) = delete;
		BLAS(BLAS&& other) noexcept;
		BLAS(const Device& device, BLASGeometry geometry);

		void Generate(
			VkCommandBuffer commandBuffer,
			const class Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			const class Buffer& blasBuffer,
			VkDeviceSize resultOffset);

	private:
		BLASGeometry geometry;
	};
}

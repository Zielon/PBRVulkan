#pragma once

#include "AccelerationStructure.h"

#include <glm/glm.hpp>
#include <vector>

namespace Vulkan
{
	// Geometry instance, with the layout expected by VK_NV_ray_tracing
	struct VkGeometryInstance
	{
		// Transform matrix, containing only the top 3 rows
		float transform[12];
		// Instance index
		uint32_t instanceCustomIndex : 24;
		// Visibility mask
		uint32_t mask : 8;
		// Index of the hit group which will be invoked when a ray hits the instance
		uint32_t instanceOffset : 24;
		// Instance flags, such as culling
		uint32_t flags : 8;
		// Opaque handle of the bottom-level acceleration structure
		uint64_t accelerationStructureHandle;
	};

	class TLAS final : public AccelerationStructure
	{
	public:
		TLAS(const TLAS&) = delete;
		TLAS& operator =(const TLAS&) = delete;
		TLAS& operator =(TLAS&&) = delete;
		TLAS(const class Device& device, const std::vector<VkGeometryInstance>& geometryInstances, bool allowUpdate);
		TLAS(TLAS&& other) noexcept;
		~TLAS() = default;

		[[nodiscard]] const std::vector<VkGeometryInstance>& GeometryInstances() const
		{
			return geometryInstances;
		}

		void Generate(
			VkCommandBuffer commandBuffer,
			class Buffer& topScratchBuffer,
			VkDeviceSize scratchOffset,
			class Buffer& topBuffer,
			VkDeviceSize topOffset,
			class Buffer& instanceBuffer,
			VkDeviceSize instanceOffset,
			bool updateOnly) const;

		static VkGeometryInstance CreateGeometryInstance(
			const class BLAS& blas,
			const glm::mat4& transform,
			uint32_t instanceId);

	private:
		std::vector<VkGeometryInstance> geometryInstances;
	};
}

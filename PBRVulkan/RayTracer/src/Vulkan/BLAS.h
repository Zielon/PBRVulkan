#pragma once

#include "AccelerationStructure.h"

#include <vector>

namespace Tracer
{
	class Scene;
}

namespace Vulkan
{
	class BLAS final : public AccelerationStructure
	{
	public:
		BLAS(const BLAS&) = delete;
		BLAS& operator =(const BLAS&) = delete;
		BLAS& operator =(BLAS&&) = delete;
		BLAS(const class Device& device, const std::vector<VkGeometryNV>& geometries, bool allowUpdate);
		BLAS(BLAS&& other) noexcept;
		~BLAS() = default;

		void Generate(
			VkCommandBuffer commandBuffer,
			const class Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			const class Buffer& blasBuffer,
			VkDeviceSize resultOffset,
			bool updateOnly) const;

		static VkGeometryNV CreateGeometry(
			const Tracer::Scene& scene,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			uint32_t indexOffset,
			uint32_t indexCount,
			bool isOpaque);

	private:
		std::vector<VkGeometryNV> geometries;
	};
}

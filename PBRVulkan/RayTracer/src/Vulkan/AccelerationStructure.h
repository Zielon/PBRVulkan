#pragma once

#include <memory>
#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	struct ASMemoryRequirements
	{
		VkMemoryRequirements Result;
		VkMemoryRequirements Build;
		VkMemoryRequirements Update;
	};
	
	class AccelerationStructure
	{
	public:
		AccelerationStructure(const AccelerationStructure&) = delete;
		AccelerationStructure& operator =(const AccelerationStructure&) = delete;
		AccelerationStructure& operator =(AccelerationStructure&&) = delete;

		AccelerationStructure(AccelerationStructure&& other) noexcept;
		virtual ~AccelerationStructure();

		static void MemoryBarrier(VkCommandBuffer commandBuffer);
		static ASMemoryRequirements GetTotalRequirements(const std::vector<ASMemoryRequirements>& requirements);
		
		[[nodiscard]] ASMemoryRequirements GetMemoryRequirements() const;

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

		[[nodiscard]] const class Extensions& GetExtensions() const
		{
			return *extensions;
		}

		[[nodiscard]] VkAccelerationStructureNV Get() const
		{
			return accelerationStructure;
		}

	protected:
		AccelerationStructure(const class Device& device, const VkAccelerationStructureCreateInfoNV& createInfo);

		const bool allowUpdate;
		const class Device& device;
		std::unique_ptr<class Extensions> extensions;
		VkAccelerationStructureNV accelerationStructure{};
	};
}

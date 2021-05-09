#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Extensions final
	{
	public:
		Extensions(const class Device& device);

		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{};

		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;

		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;

		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
		PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
		PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;

	private:
		const Device& device;
	};
}

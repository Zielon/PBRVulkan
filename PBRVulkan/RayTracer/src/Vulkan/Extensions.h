#pragma once

#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;

	class Extensions final
	{
	public:
		Extensions(const Device& device);

		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{};

		// NV_ray_tracing
		PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
		PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
		PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
		PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
		PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
		PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV{};
		PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
		PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
		PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
		PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
		PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
		PFN_vkCompileDeferredNV vkCompileDeferredNV;

	private:
		const Device& device;
	};
}

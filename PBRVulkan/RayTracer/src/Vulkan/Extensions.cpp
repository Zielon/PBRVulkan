#include "Extensions.h"

#include "Device.h"

#define GetDeviceProcAddr(name) \
	name = reinterpret_cast<PFN_##name>(vkGetDeviceProcAddr(device.Get(), #name))

namespace Vulkan
{
	Extensions::Extensions(const Device& device) : device(device)
	{
		GetDeviceProcAddr(vkSetDebugUtilsObjectNameEXT);
		GetDeviceProcAddr(vkCreateAccelerationStructureNV);
		GetDeviceProcAddr(vkDestroyAccelerationStructureNV);
		GetDeviceProcAddr(vkGetAccelerationStructureMemoryRequirementsNV);
		GetDeviceProcAddr(vkBindAccelerationStructureMemoryNV);
		GetDeviceProcAddr(vkCmdBuildAccelerationStructureNV);
		GetDeviceProcAddr(vkCmdTraceRaysNV);
		GetDeviceProcAddr(vkCreateRayTracingPipelinesNV);
		GetDeviceProcAddr(vkGetRayTracingShaderGroupHandlesNV);
		GetDeviceProcAddr(vkGetAccelerationStructureHandleNV);
		GetDeviceProcAddr(vkCmdWriteAccelerationStructuresPropertiesNV);
		GetDeviceProcAddr(vkCompileDeferredNV);
	}
}

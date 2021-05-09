#include "Extensions.h"

#include "Device.h"

#define GetDeviceProcAddr(name) \
	name = reinterpret_cast<PFN_##name>(vkGetDeviceProcAddr(device.Get(), #name))

namespace Vulkan
{
	Extensions::Extensions(const Device& device) : device(device)
	{
		GetDeviceProcAddr(vkSetDebugUtilsObjectNameEXT);

		GetDeviceProcAddr(vkCreateAccelerationStructureKHR);
		GetDeviceProcAddr(vkDestroyAccelerationStructureKHR);
		GetDeviceProcAddr(vkGetAccelerationStructureBuildSizesKHR);

		GetDeviceProcAddr(vkCreateRayTracingPipelinesKHR);
		GetDeviceProcAddr(vkGetRayTracingShaderGroupHandlesKHR);
		GetDeviceProcAddr(vkGetAccelerationStructureDeviceAddressKHR);

		GetDeviceProcAddr(vkCmdBuildAccelerationStructuresKHR);
		GetDeviceProcAddr(vkCmdCopyAccelerationStructureKHR);
		GetDeviceProcAddr(vkCmdWriteAccelerationStructuresPropertiesKHR);
		GetDeviceProcAddr(vkCmdTraceRaysKHR);
	}
}

#pragma once

#define NOMINMAX
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#undef APIENTRY

#define NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

namespace Vulkan
{
	void VK_CHECK(VkResult result, const char* operation);
}

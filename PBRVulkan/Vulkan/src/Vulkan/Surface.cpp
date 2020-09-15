#include "Surface.h"
#include "Instance.h"
#include "Window.h"

namespace Vulkan
{
	Surface::Surface(const Instance& instance): instance(instance)
	{
		VK_CHECK(glfwCreateWindowSurface(instance.Get(), instance.GetWindow().Get(), nullptr, &surface),
		         "Create window surface");
	}

	Surface::~Surface()
	{
		if (surface != nullptr)
		{
			vkDestroySurfaceKHR(instance.Get(), surface, nullptr);
			surface = nullptr;
		}
	}
}

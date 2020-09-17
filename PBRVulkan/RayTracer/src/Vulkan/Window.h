#pragma once

#include "Vulkan.h"

#include <functional>
#include <vector>

namespace Vulkan
{
	class Window final
	{
	public:
		NON_COPIABLE(Window)

		explicit Window();
		~Window();

		GLFWwindow* Get() const { return window; }
		VkExtent2D GetFramebufferSize() const;
		void Run();

	private:
		GLFWwindow* window{};
	};
}

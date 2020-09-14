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
		void Run();

	private:
		GLFWwindow* window{};
	};
}

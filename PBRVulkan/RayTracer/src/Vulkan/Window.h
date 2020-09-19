#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Window final
	{
	public:
		NON_COPIABLE(Window)

		explicit Window();
		~Window();

		[[nodiscard]] GLFWwindow* Get() const
		{
			return window;
		}

		[[nodiscard]] VkExtent2D GetFramebufferSize() const;
		void Run();

	private:

		GLFWwindow* window{};
	};
}

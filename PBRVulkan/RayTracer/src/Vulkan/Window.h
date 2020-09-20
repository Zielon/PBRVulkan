#pragma once

#include <functional>

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

		std::vector<std::function<void(int key, int scancode, int action, int mods)>> OnKeyChanged;
		std::vector<std::function<void(double xpos, double ypos)>> OnCursorPositionChanged;
		std::vector<std::function<void(int button, int action, int mods)>> OnMouseButtonChanged;
		std::vector<std::function<void(double xoffset, double yoffset)>> OnScrollChanged;
		
	private:
		GLFWwindow* window{};
	};
}

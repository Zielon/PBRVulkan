#include "Window.h"

#include <iostream>

namespace Vulkan
{
	void GlfwErrorCallback(const int error, const char* const description)
	{
		std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
	}

	void GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
	{
		auto* const me = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : me->OnKeyChanged)
			callback(key, scancode, action, mods);
	}

	void GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		auto* const me = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : me->OnCursorPositionChanged)
			callback(xpos, ypos);
	}

	void GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		auto* const me = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : me->OnMouseButtonChanged)
			callback(button, action, mods);
	}

	void GlfwScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
	{
		auto* const me = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : me->OnScrollChanged)
			callback(xoffset, yoffset);
	}

	Window::Window()
	{
		glfwSetErrorCallback(GlfwErrorCallback);

		if (!glfwInit())
		{
			throw std::runtime_error("glfwInit() failed!");
		}

		if (!glfwVulkanSupported())
		{
			throw std::runtime_error("glfwVulkanSupported() failed!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(1200, 800, "PBRVulkan", nullptr, nullptr);

		if (window == nullptr)
		{
			throw std::runtime_error("Failed to create window!");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, GlfwKeyCallback);
		glfwSetCursorPosCallback(window, GlfwCursorPositionCallback);
		glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback);
		glfwSetScrollCallback(window, GlfwScrollCallback);
	}

	Window::~Window()
	{
		if (window != nullptr)
		{
			glfwDestroyWindow(window);
			window = nullptr;
		}

		glfwTerminate();
		glfwSetErrorCallback(nullptr);
	}

	VkExtent2D Window::GetFramebufferSize() const
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}

	void Window::Run() { }
}

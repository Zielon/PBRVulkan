#include "Window.h"

#include <iostream>

namespace Vulkan
{
	void Window::GlfwErrorCallback(const int error, const char* const description)
	{
		std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
	}

	void Window::GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action,
	                             const int mods)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onKeyChanged)
			callback(key, scancode, action, mods);
	}

	void Window::GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onCursorPositionChanged)
			callback(xpos, ypos);
	}

	void Window::GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onMouseButtonChanged)
			callback(button, action, mods);
	}

	void Window::GlfwScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
	{
		auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (auto& callback : this_->onScrollChanged)
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

		window = glfwCreateWindow(800, 800, "PBRVulkan", nullptr, nullptr);

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

	void Window::AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mods)> callback)
	{
		onKeyChanged.emplace_back(callback);
	}

	void Window::AddOnCursorPositionChanged(std::function<void(double xpos, double ypos)> callback)
	{
		onCursorPositionChanged.emplace_back(callback);
	}

	void Window::AddOnMouseButtonChanged(std::function<void(int button, int action, int mods)> callback)
	{
		onMouseButtonChanged.emplace_back(callback);
	}

	void Window::AddOnScrollChanged(std::function<void(double xoffset, double yoffset)> callback)
	{
		onScrollChanged.emplace_back(callback);
	}
}

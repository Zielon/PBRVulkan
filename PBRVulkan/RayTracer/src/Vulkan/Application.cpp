#include "Application.h"

#include "Vulkan.h"
#include "Window.h"
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"

namespace Vulkan
{
	Application::Application()
	{
		const auto validationLayers = std::vector<const char*>();

		window.reset(new Window());
		instance.reset(new Instance(*window, validationLayers));
		surface.reset(new Surface(*instance));

		SetPhysicalDevice();

		swapChain.reset(new SwapChain(*device));
		graphicsPipeline.reset(new GraphicsPipeline(*swapChain, *device));
	}

	Application::~Application()
	{
		window.reset();
	}

	void Application::Run()
	{
		window->Run();
	}

	void Application::SetPhysicalDevice()
	{
		auto physicalDevice = instance->GetDevices().front();

		device.reset(new Device(physicalDevice, *surface));
	}
}

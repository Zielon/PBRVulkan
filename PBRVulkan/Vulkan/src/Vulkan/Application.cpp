#include "Application.h"

namespace Vulkan
{
	Application::Application()
	{
		const auto validationLayers = std::vector<const char*>();

		window.reset(new Window());
		instance.reset(new Instance(*window, validationLayers));
		surface.reset(new Surface(*instance));

		SetPhysicalDevice();
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

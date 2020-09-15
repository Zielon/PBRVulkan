#pragma once

#include "Vulkan.h"
#include "Window.h"
#include "Instance.h"
#include "Device.h"
#include "Surface.h"

#include <memory>

namespace Vulkan
{
	class Application
	{
	public:
		NON_COPIABLE(Application)

		explicit Application();
		virtual ~Application();

		void Run();

	private:
		void SetPhysicalDevice();

		std::unique_ptr<Window> window;
		std::unique_ptr<Instance> instance;
		std::unique_ptr<Device> device;
		std::unique_ptr<Surface> surface;
	};
}

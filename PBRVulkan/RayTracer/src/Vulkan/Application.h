#pragma once

#include "Vulkan.h"

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

		std::unique_ptr<class Window> window;
		std::unique_ptr<class Instance> instance;
		std::unique_ptr<class Device> device;
		std::unique_ptr<class Surface> surface;
		std::unique_ptr<class SwapChain> swapChain;
		std::unique_ptr<class GraphicsPipeline> graphicsPipeline;
	};
}

#pragma once

#include <memory>

#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;
	class SwapChain;
	class GraphicsPipeline;
}

namespace Tracer
{
	class Menu
	{
	public:
		NON_COPIABLE(Menu)

		Menu(const Vulkan::Device& device, const Vulkan::SwapChain& swapChain);
		~Menu();

		void Render();
	private:
		std::unique_ptr<Vulkan::GraphicsPipeline> graphicsPipeline;
	};
}

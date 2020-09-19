#pragma once

#include "Vulkan.h"

#include <memory>
#include <vector>

namespace Vulkan
{
	class Application
	{
	public:
		NON_COPIABLE(Application)

		explicit Application();
		virtual ~Application();
		void DrawFrame();
		virtual void Run();

	protected:
		void CreatePhysicalDevice();
		void CreateGraphicsPipeline();
		void CreateInstance();
		void QueueSubmit(VkCommandBuffer commandBuffer);
		void Present(uint32_t imageIndex);
		void Update();
		
		virtual void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer){};
		
		size_t currentFrame{};

		std::unique_ptr<class Window> window;
		std::unique_ptr<class Instance> instance;
		std::unique_ptr<class Device> device;
		std::unique_ptr<class Surface> surface;
		std::unique_ptr<class SwapChain> swapChain;
		std::unique_ptr<class GraphicsPipeline> graphicsPipeline;
		std::unique_ptr<class CommandBuffers> commandBuffers;

		std::vector<std::unique_ptr<class Framebuffer>> swapChainFramebuffers;
		std::vector<std::unique_ptr<class Semaphore>> imageAvailableSemaphores;
		std::vector<std::unique_ptr<class Semaphore>> renderFinishedSemaphores;
		std::vector<std::unique_ptr<class Fence>> inFlightFences;
	};
}

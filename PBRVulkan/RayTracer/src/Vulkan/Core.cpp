#include "Core.h"

#include <array>

#include "Vulkan.h"
#include "Window.h"
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "CommandBuffers.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Buffer.h"
#include "CommandPool.h"
#include "DepthBuffer.h"

#include "../Geometry/Global.h"
#include "../Tracer/Scene.h"

namespace Vulkan
{
	Core::Core()
	{
		CreateInstance();
		CreatePhysicalDevice();
	}

	Core::~Core()
	{
		commandPool.reset();
		device.reset();
		surface.reset();
		instance.reset();
		window.reset();
	}

	void Core::DrawFrame()
	{
		auto& inFlightFence = inFlightFences[currentFrame];

		inFlightFence->Wait(UINT64_MAX);

		uint32_t imageIndex;
		auto result = vkAcquireNextImageKHR(
			device->Get(),
			swapChain->Get(),
			UINT64_MAX,
			imageAvailableSemaphores[currentFrame]->Get(),
			nullptr,
			&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error(std::string("Failed to acquire next image"));
		}

		const VkCommandBuffer commandBuffer = commandBuffers->Begin(imageIndex);
		{
			Render(swapChainFrameBuffers[imageIndex]->Get(), commandBuffer, imageIndex);
		}
		commandBuffers->End(imageIndex);

		inFlightFence->Reset();

		UpdateUniformBuffer(imageIndex);

		QueueSubmit(commandBuffer);
		Present(imageIndex);

		currentFrame = (currentFrame + 1) % inFlightFences.size();
	}

	void Core::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			DrawFrame();
		}
	}

	void Core::Present(uint32_t imageIndex)
	{
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]->Get() };
		VkSwapchainKHR swapChains[] = { swapChain->Get() };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		auto result = vkQueuePresentKHR(device->PresentQueue, &presentInfo);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error(std::string("Failed to present next image"));
		}
	}

	void Core::QueueSubmit(VkCommandBuffer commandBuffer)
	{
		auto& fence = inFlightFences[currentFrame];

		VkCommandBuffer commandBuffers[]{ commandBuffer };
		VkSubmitInfo submitInfo{};
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]->Get() };
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]->Get() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(device->GraphicsQueue, 1, &submitInfo, fence->Get()), "Failed to submit!");
	}

	void Core::CreateInstance()
	{
#ifdef NDEBUG
		std::vector<const char*> layers = {};
#else
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
#endif

		// Frame pre second monitor
		layers.push_back("VK_LAYER_LUNARG_monitor");

		window.reset(new Window());
		instance.reset(new Instance(*window, layers));
		surface.reset(new Surface(*instance));
	}

	void Core::CreatePhysicalDevice()
	{
		auto* physicalDevice = instance->GetDevices().front();

		device.reset(new Device(physicalDevice, *surface));
		commandPool.reset(new CommandPool(*device));
	}
}

#include "Application.h"

#include <array>

#include "Vulkan.h"
#include "Window.h"
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "CommandBuffers.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Framebuffer.h"

namespace Vulkan
{
	Application::Application()
	{
		CreateInstance();
		CreatePhysicalDevice();
		CreateGraphicsPipeline();
	}

	Application::~Application()
	{
		window.reset();
	}

	void Application::DrawFrame()
	{
		uint32_t imageIndex;
		auto& fence = inFlightFences[currentFrame];
		auto& semaphore = semaphores[currentFrame];

		fence->Wait(UINT64_MAX);

		vkAcquireNextImageKHR(
			device->Get(),
			swapChain->Get(),
			UINT64_MAX,
			semaphore->GetImageAvailable(),
			fence->Get(),
			&imageIndex);

		const auto framebuffer = swapChainFramebuffers[imageIndex]->Get();
		
		const VkCommandBuffer command = commandBuffers->Begin(imageIndex);
		Render(framebuffer, command);
		commandBuffers->End(imageIndex);

		QueueSubmit(command);
		fence->Reset();
		Present(imageIndex);
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			DrawFrame();
		}
	}

	void Application::Present(uint32_t imageIndex)
	{
		auto& semaphore = semaphores[currentFrame];

		VkSemaphore waitSemaphores[] = {semaphore->GetImageAvailable()};
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphores;

		VkSwapchainKHR swapChains[] = {swapChain->Get()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(device->PresentQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % inFlightFences.size();
	}

	void Application::CreateInstance()
	{
		const auto validationLayers = std::vector<const char*>();

		window.reset(new Window());
		instance.reset(new Instance(*window, validationLayers));
		surface.reset(new Surface(*instance));
	}

	void Application::QueueSubmit(VkCommandBuffer command)
	{
		auto& fence = inFlightFences[currentFrame];
		auto& semaphore = semaphores[currentFrame];

		VkCommandBuffer commandBuffers[]{command};
		VkSubmitInfo submitInfo{};
		VkSemaphore signalSemaphores[] = {semaphore->GetRenderFinished()};
		VkSemaphore waitSemaphores[] = {semaphore->GetImageAvailable()};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

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

	void Application::CreatePhysicalDevice()
	{
		auto physicalDevice = instance->GetDevices().front();

		device.reset(new Device(physicalDevice, *surface));
	}

	void Application::CreateGraphicsPipeline()
	{
		swapChain.reset(new SwapChain(*device));
		graphicsPipeline.reset(new GraphicsPipeline(*swapChain, *device));

		for (const auto& imageView : swapChain->GetSwapChainImageViews())
		{
			semaphores.emplace_back(new Semaphore(*device));
			inFlightFences.emplace_back(new Fence(*device));
			swapChainFramebuffers.emplace_back(
				new Framebuffer(*imageView, *swapChain, graphicsPipeline->GetRenderPass()));
		}

		commandBuffers.reset(new CommandBuffers(*device, swapChainFramebuffers.size()));
	}
}

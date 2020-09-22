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
		// Destructor has to call in the right order all graphics components
		commandBuffers.reset();
		swapChainFramebuffers.clear();
		graphicsPipeline.reset();
		inFlightFences.clear();
		renderFinishedSemaphores.clear();
		imageAvailableSemaphores.clear();
		swapChain.reset();
		commandBuffers.reset();
		device.reset();
		surface.reset();
		instance.reset();
		window.reset();
	}

	void Application::DrawFrame()
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
			Render(swapChainFramebuffers[imageIndex]->Get(), commandBuffer);
		}
		commandBuffers->End(imageIndex);

		inFlightFence->Reset();

		QueueSubmit(commandBuffer);
		Present(imageIndex);
		Update();
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

	void Application::Update()
	{
		currentFrame = (currentFrame + 1) % inFlightFences.size();
	}

	void Application::CreateInstance()
	{
#ifdef NDEBUG
		const std::vector<const char*> validationLayers = {};
#else
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
		window.reset(new Window());
		instance.reset(new Instance(*window, validationLayers));
		surface.reset(new Surface(*instance));
	}

	void Application::QueueSubmit(VkCommandBuffer commandBuffer)
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

	void Application::CreatePhysicalDevice()
	{
		auto* physicalDevice = instance->GetDevices().front();

		device.reset(new Device(physicalDevice, *surface));
	}

	void Application::CreateGraphicsPipeline()
	{
		swapChain.reset(new SwapChain(*device));

		for (const auto& imageView : swapChain->GetImageViews())
		{
			imageAvailableSemaphores.emplace_back(new Semaphore(*device));
			renderFinishedSemaphores.emplace_back(new Semaphore(*device));
			inFlightFences.emplace_back(new Fence(*device));
		}

		graphicsPipeline.reset(new GraphicsPipeline(*swapChain, *device));

		for (const auto& imageView : swapChain->GetImageViews())
		{
			swapChainFramebuffers.emplace_back(
				new Framebuffer(*imageView, *swapChain, graphicsPipeline->GetRenderPass()));
		}

		commandBuffers.reset(new CommandBuffers(*device, static_cast<uint32_t>(swapChainFramebuffers.size())));
	}
}

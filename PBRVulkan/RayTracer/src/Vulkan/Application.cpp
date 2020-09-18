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

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace Vulkan
{
	Application::Application()
	{
		const auto validationLayers = std::vector<const char*>();

		window.reset(new Window());
		instance.reset(new Instance(*window, validationLayers));
		surface.reset(new Surface(*instance));

		CreatePhysicalDevice();
		CreateGraphicsPipeline();
	}

	Application::~Application()
	{
		window.reset();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			DrawFrame();
		}
	}

	void Application::DrawFrame()
	{
		auto& fence = inFlightFences[currentFrame];
		
		fence->Wait(UINT64_MAX);

		auto& semaphore = semaphores[currentFrame];
		auto imageAvailableSemaphore = semaphore->GetImageAvailable();
		auto renderFinishedSemaphore = semaphore->GetRenderFinished();
		
		uint32_t imageIndex;
		auto result = vkAcquireNextImageKHR(
			device->Get(), swapChain->Get(), UINT64_MAX, imageAvailableSemaphore, nullptr, &imageIndex);

		// ============= START COMMAND =============
		
		const auto commandBuffer = commandBuffers->Begin(imageIndex);

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = graphicsPipeline->GetRenderPass();
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex]->Get();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain->Extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		}
		vkCmdEndRenderPass(commandBuffer);

		commandBuffers->End(imageIndex);

		// ============= END COMMAND =============
		
		VkCommandBuffer commandBuffers[]{commandBuffer};
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;

		VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		fence->Reset();

		if (vkQueueSubmit(device->GraphicsQueue, 1, &submitInfo, fence->Get()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {swapChain->Get()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(device->PresentQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % inFlightFences.size();
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
			swapChainFramebuffers.emplace_back(new Framebuffer(*imageView, *swapChain, graphicsPipeline->GetRenderPass()));
		}

		commandBuffers.reset(new CommandBuffers(*device, swapChainFramebuffers.size()));
	}
}

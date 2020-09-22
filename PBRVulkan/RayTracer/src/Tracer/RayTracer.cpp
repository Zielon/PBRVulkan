#include "RayTracer.h"

#include <array>
#include <iostream>

#include "Menu.h"
#include "Scene.h"

#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Window.h"

namespace Tracer
{
	RayTracer::RayTracer()
	{
		menu.reset(new Menu(*device, *swapChain, *commandBuffers));
		scene.reset(new Scene(*device, *commandBuffers));

		RegisterCallbacks();
	}

	void RayTracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer)
	{
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = graphicsPipeline->GetRenderPass();
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->Extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkBuffer vertexBuffers[] = { scene->GetVertexBuffer().Get() };
		const VkBuffer indexBuffer = scene->GetIndexBuffer().Get();
		VkDeviceSize offsets[] = { 0 };

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(commandBuffer);

		menu->Render(framebuffer, commandBuffer);
	}

	void RayTracer::RegisterCallbacks()
	{
		window->OnCursorPositionChanged.emplace_back([this](const double xpos, const double ypos)-> void
		{
			OnCursorPositionChanged(xpos, ypos);
		});

		window->OnKeyChanged.emplace_back(
			[this](const int key, const int scancode, const int action, const int mods)-> void
			{
				OnKeyChanged(key, scancode, action, mods);
			});

		window->OnMouseButtonChanged.emplace_back([this](const int button, const int action, const int mods)-> void
		{
			OnMouseButtonChanged(button, action, mods);
		});

		window->OnScrollChanged.emplace_back([this](const double xoffset, const double yoffset)-> void
		{
			OnScrollChanged(xoffset, yoffset);
		});
	}

	void RayTracer::OnKeyChanged(int key, int scancode, int action, int mods)
	{
		if (menu->WantCaptureKeyboard())
			return;
	}

	void RayTracer::OnCursorPositionChanged(double xpos, double ypos)
	{
		if (menu->WantCaptureKeyboard() || menu->WantCaptureMouse())
			return;
	}

	void RayTracer::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (menu->WantCaptureMouse())
			return;
	}

	void RayTracer::OnScrollChanged(double xoffset, double yoffset)
	{
		if (menu->WantCaptureMouse())
			return;
	}

	void RayTracer::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			DrawFrame();
		}

		device->WaitIdle();
	}

	RayTracer::~RayTracer() { }
}

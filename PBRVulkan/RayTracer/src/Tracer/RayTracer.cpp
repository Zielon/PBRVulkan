#include "RayTracer.h"

#include <array>
#include <iostream>


#include "Menu.h"

#include "../Vulkan/Device.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Window.h"

namespace Tracer
{
	RayTracer::RayTracer()
	{
		menu.reset(new Menu(*device, *swapChain, *commandBuffers));

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

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		}
		vkCmdEndRenderPass(commandBuffer);

		menu->Render(framebuffer, commandBuffer);
	}

	void RayTracer::RegisterCallbacks()
	{
		window->OnCursorPositionChanged.emplace_back([this](const double xpos, const double ypos)
			{
				OnCursorPositionChanged(xpos, ypos);
			});

		window->OnKeyChanged.emplace_back([this](const int key, const int scancode, const int action, const int mods)
			{
				OnKeyChanged(key, scancode, action, mods);
			});

		window->OnMouseButtonChanged.emplace_back([this](const int button, const int action, const int mods)
			{
				OnMouseButtonChanged(button, action, mods);
			});

		window->OnScrollChanged.emplace_back([this](const double xoffset, const double yoffset)
			{
				OnScrollChanged(xoffset, yoffset);
			});
	}

	
	void RayTracer::OnKeyChanged(int key, int scancode, int action, int mods)
	{
		std::cout << "NOT IMPLEMENTED OnKeyChanged()" << std::endl;
	}
	
	void RayTracer::OnCursorPositionChanged(double xpos, double ypos)
	{
		std::cout << "NOT IMPLEMENTED OnCursorPositionChanged()" << std::endl;
	}
	
	void RayTracer::OnMouseButtonChanged(int button, int action, int mods)
	{
		std::cout << "NOT IMPLEMENTED OnMouseButtonChanged()" << std::endl;
	}
	
	void RayTracer::OnScrollChanged(double xoffset, double yoffset)
	{
		std::cout << "NOT IMPLEMENTED OnScrollChanged()" << std::endl;
	}

	void RayTracer::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			DrawFrame();
		}
	}

	RayTracer::~RayTracer() { }
}

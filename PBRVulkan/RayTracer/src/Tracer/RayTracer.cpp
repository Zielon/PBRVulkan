#include "RayTracer.h"

#include <array>
#include <iostream>

#include "Menu.h"

#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Window.h"

namespace Tracer
{
	RayTracer::RayTracer()
	{
		menu.reset(new Menu(*device, *swapChain, *commandBuffers));

		const std::vector<Geometry::Vertex> vertices = {
			{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -0.5f } },
			{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } },
			{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { -0.5f, 0.5f } }
		};

		const auto size = sizeof(vertices[0]) * vertices.size();

		std::unique_ptr<Vulkan::Buffer<Geometry::Vertex>> buffer_staging(
			new Vulkan::Buffer<Geometry::Vertex>(
				*device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(vertices.data());

		vertexBuffer.reset(
			new Vulkan::Buffer<Geometry::Vertex>(
				*device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		vertexBuffer->Copy(*commandBuffers, *buffer_staging);

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
			VkBuffer vertexBuffers[] = { vertexBuffer->Get() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(3), 1, 0, 0);
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
	}

	RayTracer::~RayTracer() { }
}

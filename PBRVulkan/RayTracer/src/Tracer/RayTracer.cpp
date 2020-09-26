#include "RayTracer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>

#include "Menu.h"
#include "Scene.h"

#include "../Geometry/MVP.h"

#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Window.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/DescriptorsManager.h"

namespace Tracer
{
	RayTracer::RayTracer()
	{
		LoadScene();
		CreateSwapChain();
		CreateGraphicsPipeline(*scene);
		RegisterCallbacks();

		menu.reset(new Menu(*device, *swapChain, *commandPool));
	}

	void RayTracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
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
		VkDescriptorSet descriptorSets[] = { descriptorsManager->GetDescriptorSets()[imageIndex] };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                        graphicsPipeline->GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, scene->GetIndexSize(), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(commandBuffer);

		menu->Render(framebuffer, commandBuffer);
	}

	void RayTracer::LoadScene()
	{
		scene.reset(new Scene(*device, *commandPool));
	}

	void RayTracer::UpdateUniformBuffer(uint32_t imageIndex)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		Uniforms::MVP ubo{};
		ubo.Model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.View = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.Projection = glm::perspective(glm::radians(45.0f),
		                                  swapChain->Extent.width / static_cast<float>(swapChain->Extent.height), 0.1f,
		                                  10.0f);
		ubo.Projection[1][1] *= -1;

		uniformBuffers[imageIndex]->Fill(&ubo);
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

#include "RayTracer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>

#include "Menu.h"
#include "Scene.h"
#include "Camera.h"

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
		Camera::UpdateTime();

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

			uint32_t vertexOffset = 0;
			uint32_t indexOffset = 0;

			// Render all models with proper buffers offsets
			for (const auto& mesh : scene->GetMeshes())
			{
				const uint32_t vertices = mesh->GetVerticesSize();
				const uint32_t indecies = mesh->GetIndeciesSize();

				vkCmdDrawIndexed(commandBuffer, indecies, 1, indexOffset, vertexOffset, 0);

				vertexOffset += vertices;
				indexOffset += indecies;
			}
		}
		vkCmdEndRenderPass(commandBuffer);

		scene->GetCamera().OnEventChanged();

		menu->Render(framebuffer, commandBuffer);
	}

	void RayTracer::LoadScene()
	{
		//const std::string CONFIG = "../Assets/Scenes/cornell_box.scene";
		const std::string CONFIG = "../Assets/Scenes/coffee_cart.scene";
		scene.reset(new Scene(CONFIG, *device, *commandPool));
	}

	void RayTracer::UpdateUniformBuffer(uint32_t imageIndex)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		Uniforms::MVP ubo{};
		ubo.view = scene->GetCamera().GetView();
		ubo.projection = scene->GetCamera().GetProjection();
		ubo.direction = scene->GetCamera().GetDirection();

		uniformBuffers[imageIndex]->Fill(&ubo);
	}

	void RayTracer::RegisterCallbacks()
	{
		window->AddOnCursorPositionChanged([this](const double xpos, const double ypos)-> void
		{
			OnCursorPositionChanged(xpos, ypos);
		});

		window->AddOnKeyChanged([this](const int key, const int scancode, const int action, const int mods)-> void
		{
			OnKeyChanged(key, scancode, action, mods);
		});

		window->AddOnMouseButtonChanged([this](const int button, const int action, const int mods)-> void
		{
			OnMouseButtonChanged(button, action, mods);
		});

		window->AddOnScrollChanged([this](const double xoffset, const double yoffset)-> void
		{
			OnScrollChanged(xoffset, yoffset);
		});
	}

	void RayTracer::OnKeyChanged(int key, int scanCode, int action, int mods)
	{
		if (menu->WantCaptureKeyboard() || !swapChain)
			return;

		scene->GetCamera().OnKeyChanged(key, scanCode, action, mods);
	}

	void RayTracer::OnCursorPositionChanged(double xpos, double ypos)
	{
		if (menu->WantCaptureKeyboard() || menu->WantCaptureMouse() || !swapChain)
			return;

		scene->GetCamera().OnCursorPositionChanged(xpos, ypos);
	}

	void RayTracer::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (menu->WantCaptureMouse() || !swapChain)
			return;

		scene->GetCamera().OnMouseButtonChanged(button, action, mods);
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

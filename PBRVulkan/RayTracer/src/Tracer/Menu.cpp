#include "Menu.h"

#include <array>
#include <iostream>
#include <ostream>

#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/Instance.h"
#include "../Vulkan/Window.h"
#include "../Vulkan/CommandPool.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Command.cpp"

#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_vulkan.h"

namespace Tracer
{
	Menu::Menu(
		const Vulkan::Device& device,
		const Vulkan::SwapChain& swapChain,
		const Vulkan::CommandPool& commandPool)
		: swapChain(swapChain), device(device)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto* window = device.GetSurface().GetInstance().GetWindow().Get();
		renderPass.reset(new Vulkan::RenderPass(device, swapChain, false, false));

		if (!ImGui_ImplGlfw_InitForVulkan(window, false))
		{
			throw std::runtime_error("Failed to initialise ImGui GLFW adapter!");
		}

		VkDescriptorPoolSize descriptor{};
		descriptor.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor.descriptorCount = 1;

		std::array<VkDescriptorPoolSize, 1> poolSizes = { descriptor };

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &descriptorPool);

		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = device.GetSurface().GetInstance().Get();
		vulkanInit.PhysicalDevice = device.GetPhysical();
		vulkanInit.Device = device.Get();
		vulkanInit.QueueFamily = device.GraphicsFamilyIndex;
		vulkanInit.Queue = device.GraphicsQueue;
		vulkanInit.PipelineCache = nullptr;
		vulkanInit.DescriptorPool = descriptorPool;
		vulkanInit.MinImageCount = swapChain.MinImageCount;
		vulkanInit.ImageCount = static_cast<uint32_t>(swapChain.GetImage().size());
		vulkanInit.Allocator = nullptr;

		if (!ImGui_ImplVulkan_Init(&vulkanInit, renderPass->Get()))
		{
			throw std::runtime_error("Failed to initialise ImGui vulkan adapter!");
		}

		auto& io = ImGui::GetIO();

		if (!io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Cousine-Regular.ttf", 13))
		{
			throw std::runtime_error("failed to load ImGui font");
		}

		ImGui::StyleColorsDark();
		Vulkan::Command::Submit(commandPool, [](VkCommandBuffer commandBuffer)
		{
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		});
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	Menu::~Menu()
	{
		vkDestroyDescriptorPool(device.Get(), descriptorPool, nullptr);
		descriptorPool = nullptr;
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Menu::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer) const
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();
		RenderSettings();
		ImGui::Render();

		std::array<VkClearValue, 2> clearValues = {};

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->Get();
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain.Extent;
		renderPassInfo.clearValueCount = 0;
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		}
		vkCmdEndRenderPass(commandBuffer);
	}

	void Menu::RenderSettings() const
	{
		bool open = true;
		const float distance = 10.0f;
		const ImVec2 pos = ImVec2(distance, distance);
		const ImVec2 posPivot = ImVec2(0.0f, 0.0f);
		const auto flags =
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);

		if (ImGui::Begin("Settings", &open, flags))
		{
			ImGui::Text("Controls");
			ImGui::Separator();
			ImGui::Text("Help");
			ImGui::Separator();
			ImGui::Text("Scene");
			ImGui::Separator();
		}

		ImGui::End();
	}
}

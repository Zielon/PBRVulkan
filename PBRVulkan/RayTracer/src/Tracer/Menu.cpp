#include "Menu.h"

#include <array>

#include "imgui.h"

#include "../Vulkan/SwapChain.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/Instance.h"
#include "../Vulkan/Window.h"
#include "../Vulkan/GraphicsPipeline.h"

#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_vulkan.h"

namespace Tracer
{
	Menu::Menu(const Vulkan::Device& device, const Vulkan::SwapChain& swapChain)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		if (!ImGui_ImplGlfw_InitForVulkan(device.GetSurface().GetInstance().GetWindow().Get(), false))
		{
			throw std::runtime_error("Failed to initialise ImGui GLFW adapter!");
		}

		graphicsPipeline.reset(new Vulkan::GraphicsPipeline(swapChain, device));

		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = device.GetSurface().GetInstance().Get();
		vulkanInit.PhysicalDevice = device.GetPhysical();
		vulkanInit.Device = device.Get();
		vulkanInit.QueueFamily = device.GraphicsFamilyIndex;
		vulkanInit.Queue = device.GraphicsQueue;
		vulkanInit.PipelineCache = nullptr;
		//vulkanInit.DescriptorPool = 
		vulkanInit.MinImageCount = swapChain.MinImageCount;
		vulkanInit.ImageCount = static_cast<uint32_t>(swapChain.GetSwapChainImages().size());
		vulkanInit.Allocator = nullptr;

		if (!ImGui_ImplVulkan_Init(&vulkanInit, graphicsPipeline->GetRenderPass()))
		{
			throw std::runtime_error("Failed to initialise ImGui vulkan adapter!");
		}
	}

	Menu::~Menu()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Menu::Render()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();

		ImGui::Render();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
	}
}

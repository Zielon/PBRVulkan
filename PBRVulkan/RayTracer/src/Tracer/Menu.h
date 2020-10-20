#pragma once

#include <imgui.h>
#include <memory>

#include "Compiler.h"
#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;
	class SwapChain;
	class RenderPass;
	class CommandPool;
}

namespace Tracer
{
	struct Settings
	{
		bool UseRasterizer{};
		int IntegratorType{};
		int SceneId{};
		int MaxDepth{};
		float Fov{};
	};

	class Menu
	{
	public:
		NON_COPIABLE(Menu)

		Menu(
			const Vulkan::Device& device,
			const Vulkan::SwapChain& swapChain,
			const Vulkan::CommandPool& commandPool,
			Settings settings);
		~Menu();

		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer);

		[[nodiscard]] const Settings& GetSettings() const
		{
			return settings;
		};

		static bool WantCaptureMouse()
		{
			return ImGui::GetIO().WantCaptureMouse;
		}

		static bool WantCaptureKeyboard()
		{
			return ImGui::GetIO().WantCaptureKeyboard;
		}

	private:
		void RenderSettings();

		Settings settings;
		VkDescriptorPool descriptorPool;
		const Vulkan::SwapChain& swapChain;
		const Vulkan::Device& device;
		std::unique_ptr<Vulkan::RenderPass> renderPass;
	};
}

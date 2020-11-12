#pragma once

#include <imgui.h>
#include <memory>

#include "Compiler.h"
#include "../Vulkan/Vulkan.h"

namespace Interface
{
	class Widget;
}

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
		bool UseGammaCorrection = true;
		bool UseDenoiser = false;
		int IntegratorType{};
		int SceneId = 5;
		int MaxDepth = 3;
		int SSP = 1;
		float Fov{};
		float Aperture = 0.001f;
		float FocalDistance = 1.f;
		float AORayLength = 0.5f;
		float DenoiseStrength = 1.f;

		[[nodiscard]] bool RequiresShaderRecompliation(const Settings& prev) const
		{
			return UseGammaCorrection != prev.UseGammaCorrection || IntegratorType != prev.IntegratorType || UseDenoiser
				!= prev.UseDenoiser;
		}

		[[nodiscard]] bool RequiresAccumulationReset(const Settings& prev) const
		{
			return
				UseRasterizer != prev.UseRasterizer ||
				UseGammaCorrection != prev.UseGammaCorrection ||
				IntegratorType != prev.IntegratorType ||
				SceneId != prev.SceneId ||
				MaxDepth != prev.MaxDepth ||
				SSP != prev.SSP ||
				Fov != prev.Fov ||
				Aperture != prev.Aperture ||
				FocalDistance != prev.FocalDistance ||
				AORayLength != prev.AORayLength ||
				DenoiseStrength != prev.DenoiseStrength;
		}
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

		void AddWidget(const std::shared_ptr<Interface::Widget>& widget);

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
		std::vector<std::shared_ptr<Interface::Widget>> widgets;
	};
}

#pragma once

#include "Menu.h"

#include "../Vulkan/Raytracer.h"
#include "../Vulkan/Computer.h"

namespace Tracer
{
	/**
	 * The inheritance chain is as follows:
	 *
	 * [Application -> Raytracer -> Rasterizer -> Core]
	 */
	class Application final : public Vulkan::Raytracer
	{
	public:
		Application();
		~Application();

		void Run() override;

	private:
		void RegisterCallbacks();
		void UpdateUniformBuffer(uint32_t imageIndex) override;
		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex) override;
		void LoadScene() override;
		void UpdateSettings();
		void CompileShaders() const;
		void RecreateSwapChain();
		void RecompileShaders();
		void CreateMenu();
		void ResetAccumulation();
		void ResizeWindow() const;
		void PrintGPUInfo() const;
		void CreateComputePipeline();
		void Postprocess(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;
		
		// User interface API
		void OnKeyChanged(int key, int scanCode, int action, int mods) override;
		void OnCursorPositionChanged(double xpos, double ypos) override;
		void OnMouseButtonChanged(int button, int action, int mods) override;
		void OnScrollChanged(double xoffset, double yoffset) override;

		Settings settings;
		std::unique_ptr<class Menu> menu;
		std::unique_ptr<class Compiler> compiler;
		std::unique_ptr<class Vulkan::Computer> computer;

		uint32_t frame = 0;
	};
}

#pragma once

#include "../Vulkan/Raytracer.h"

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

		// User interface API
		void OnKeyChanged(int key, int scanCode, int action, int mods) override;
		void OnCursorPositionChanged(double xpos, double ypos) override;
		void OnMouseButtonChanged(int button, int action, int mods) override;
		void OnScrollChanged(double xoffset, double yoffset) override;

		std::unique_ptr<class Menu> menu;
	};
}

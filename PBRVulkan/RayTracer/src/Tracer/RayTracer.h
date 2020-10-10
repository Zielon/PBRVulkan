#pragma once

#include "../Vulkan/Application.h"

namespace Tracer
{
	class RayTracer final : public Vulkan::Application
	{
	public:
		RayTracer();
		~RayTracer();

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
		std::unique_ptr<class Scene> scene;
	};
}

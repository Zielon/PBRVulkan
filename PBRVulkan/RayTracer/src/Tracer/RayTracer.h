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
		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer) override;
		std::unique_ptr<class Menu> menu;
	};
}

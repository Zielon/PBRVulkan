#pragma once

#include "Rasterizer.h"
#include "RaytracerGraphicsPipeline.h"

namespace Vulkan
{
	/*
	 * Raytracer assumes that swap chain is already initialized by rasterizer.
	 */
	class Raytracer : public Rasterizer
	{
	public:
		NON_COPIABLE(Raytracer)

		Raytracer();
		~Raytracer();

		void CreateSwapChain() override;
		void DeleteSwapChain() override;

	protected:
		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

	private:
		std::unique_ptr<class RaytracerGraphicsPipeline> raytracerGraphicsPipeline;
	};
}

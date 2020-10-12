#include "Raytracer.h"

namespace Vulkan
{
	Raytracer::Raytracer() { }

	Raytracer::~Raytracer()
	{
		Raytracer::DeleteSwapChain();
	}

	void Raytracer::CreateSwapChain()
	{
		Rasterizer::CreateSwapChain();

		raytracerGraphicsPipeline.reset(new RaytracerGraphicsPipeline(*swapChain, *device, *scene, uniformBuffers));
	}

	void Raytracer::DeleteSwapChain() {}

	void Raytracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		// TODO
	}
}

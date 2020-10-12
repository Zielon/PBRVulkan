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

		// TODO
	}

	void Raytracer::DeleteSwapChain() {}

	void Raytracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		// TODO
	}
}

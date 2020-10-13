#include "Raytracer.h"

#include "ShaderBindingTable.h"
#include "RaytracerGraphicsPipeline.h"

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
		shaderBindingTable.reset(new ShaderBindingTable(*raytracerGraphicsPipeline));
	}

	void Raytracer::DeleteSwapChain() {}

	void Raytracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		// TODO
	}
}

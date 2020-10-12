#include "RaytracerGraphicsPipeline.h"

#include "SwapChain.h"
#include "Device.h"
#include "Shader.h"
#include "RenderPass.h"
#include "DescriptorSetLayout.h"
#include "DescriptorsManager.h"
#include "Buffer.h"

#include "../Tracer/Scene.h"
#include "../Tracer/TextureImage.h"

namespace Vulkan
{
	RaytracerGraphicsPipeline::RaytracerGraphicsPipeline(
		const SwapChain& swapChain,
		const Device& device,
		const Tracer::Scene& scene,
		const std::vector<std::unique_ptr<Buffer>>& uniformBuffers):
		device(device),
		swapChain(swapChain),
		renderPass(new RenderPass(device, swapChain, true, true)) { }

	RaytracerGraphicsPipeline::~RaytracerGraphicsPipeline() { }
}

#include "Computer.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Device.h"
#include "SwapChain.h"
#include "Image.h"
#include "ImageView.h"
#include "ComputePipeline.h"
#include "CommandPool.h"
#include "CommandBuffers.h"
#include "Semaphore.h"
#include "Buffer.h"
#include "Command.cpp"

#include "../Geometry/Denoiser.h"

namespace Vulkan
{
	Computer::Computer(const SwapChain& swapChain,
	                   const Device& device,
	                   const ImageView& inputImageView,
	                   const ImageView& normalsImageView,
	                   const ImageView& positionsImageView):

		inputImageView(inputImageView), swapChain(swapChain),
		device(device),
		normalsImageView(normalsImageView),
		positionsImageView(positionsImageView)
	{
		commandPool.reset(new CommandPool(device, device.ComputeFamilyIndex));
		commandBuffers.reset(new CommandBuffers(*commandPool, static_cast<uint32_t>(swapChain.GetImage().size())));
		semaphore.reset(new Semaphore(device));

		CreateOutputTexture();
		CreateUniformBuffer();

		computePipeline.reset(new ComputePipeline(
			swapChain,
			device,
			inputImageView,
			*outputImageView,
			normalsImageView,
			positionsImageView,
			uniformBuffers));
	}

	Computer::~Computer()
	{
		computePipeline.reset();
		commandBuffers.reset();
		commandPool.reset();

		std::cout << "[COMPUTER] Compute pipeline has been deleted." << std::endl;
	}

	//void Computer::Denoise() const
	//{
	//	float colorSigma = 1.f;
	//	float positionSigma = 1.f;
	//	float normalSigma = 0.2f;
	//	const int iterations = 3;

	//	for (int i = 0; i < iterations; ++i)
	//	{
	//		Uniforms::Denoiser uniform;

	//		float scale = powf(2.f, -static_cast<float>(i));
	//		float stepWidth = powf(2.f, static_cast<float>(i));
	//		float colSigmaI = scale * colorSigma;
	//		float posSigmaI = scale * positionSigma;
	//		float norSigmaI = scale * normalSigma;

	//		uniform.iteration = i;
	//		uniform.stepWidth = stepWidth;
	//		uniform.positionPhi = posSigmaI * posSigmaI;
	//		uniform.colorPhi = colSigmaI * colSigmaI;
	//		uniform.normalPhi = norSigmaI * norSigmaI;

	//		uniformBuffers[0]->Fill(&uniform);

	//		VkImageSubresourceRange subresourceRange;

	//		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//		subresourceRange.baseMipLevel = 0;
	//		subresourceRange.levelCount = 1;
	//		subresourceRange.baseArrayLayer = 0;
	//		subresourceRange.layerCount = 1;

	//		const auto extent = swapChain.Extent;

	//		VkDescriptorSet descriptorSets[] = { computePipeline->GetDescriptorSets()[0] };

	//		VkCommandBuffer commandBuffer = commandBuffers->Begin(0);
	//		{
	//			Image::MemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange, 0,
	//			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
	//			                     VK_IMAGE_LAYOUT_GENERAL);

	//			Image::MemoryBarrier(commandBuffer, inputImageView.GetImage(), subresourceRange, 0,
	//			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
	//			                     VK_IMAGE_LAYOUT_GENERAL);

	//			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
	//			                  computePipeline->GetComputePipelines()[0]);

	//			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
	//			                        computePipeline->GetPipelineLayout(),
	//			                        0, 1, descriptorSets, 0, nullptr);

	//			vkCmdDispatch(commandBuffer, extent.width / 16, extent.height / 16, 1);
	//		}
	//		commandBuffers->End(0);

	//		VkSemaphore signalSemaphores[] = { semaphore->Get() };

	//		VkSubmitInfo submitInfo{};
	//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//		submitInfo.commandBufferCount = 1;
	//		submitInfo.pCommandBuffers = &commandBuffer;
	//		submitInfo.pSignalSemaphores = signalSemaphores;

	//		vkQueueSubmit(device.ComputeQueue, 1, &submitInfo, nullptr);
	//		vkQueueWaitIdle(device.ComputeQueue);
	//	}
	//}

	void Computer::CreateOutputTexture()
	{
		const auto extent = swapChain.Extent;
		const auto outputFormat = swapChain.Format;

		outputImage.reset(
			new Image(device, extent, outputFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_TYPE_2D,
			          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		outputImageView.reset(new ImageView(device, outputImage->Get(), outputFormat));
	}

	void Computer::CreateUniformBuffer()
	{
		uniformBuffers.emplace_back(
			new Buffer(device, sizeof(Uniforms::Denoiser),
			           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	}
}

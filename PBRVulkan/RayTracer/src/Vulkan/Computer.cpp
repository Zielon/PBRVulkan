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

#include "../Geometry/Compute.h"

namespace Vulkan
{
	Computer::Computer(const SwapChain& swapChain,
	                   const Device& device,
	                   const ImageView& inputImageView,
	                   const ImageView& normalsImageView,
	                   const ImageView& positionsImageView):

		swapChain(swapChain), device(device),
		inputImageView(inputImageView),
		normalsImageView(normalsImageView),
		positionsImageView(positionsImageView)
	{
		commandPool.reset(new CommandPool(device, device.ComputeFamilyIndex));
		commandBuffers.reset(new CommandBuffers(*commandPool, 1));
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

	void Computer::BuildCommand(int32_t shaderId)
	{
		if (shaderId == currentShader)
			return;

		currentShader = shaderId;

		vkQueueWaitIdle(device.ComputeQueue);

		const auto extent = swapChain.Extent;

		VkDescriptorSet descriptorSets[] = { computePipeline->GetDescriptorSets()[0] };

		VkCommandBuffer commandBuffer = commandBuffers->Begin(0);
		{
			VkImageSubresourceRange subresourceRange = Image::GetSubresourceRange();

			Image::MemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange, 0,
			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                     VK_IMAGE_LAYOUT_GENERAL);

			Image::MemoryBarrier(commandBuffer, normalsImageView.GetImage(), subresourceRange, 0,
			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                     VK_IMAGE_LAYOUT_GENERAL);

			Image::MemoryBarrier(commandBuffer, positionsImageView.GetImage(), subresourceRange, 0,
			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                     VK_IMAGE_LAYOUT_GENERAL);

			Image::MemoryBarrier(commandBuffer, inputImageView.GetImage(), subresourceRange, 0,
			                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                     VK_IMAGE_LAYOUT_GENERAL);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
			                  computePipeline->GetComputePipelines()[currentShader]);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
			                        computePipeline->GetPipelineLayout(),
			                        0, 1, descriptorSets, 0, nullptr);

			vkCmdDispatch(commandBuffer, extent.width / 16, extent.height / 16, 1);
		}
		commandBuffers->End(0);
	}

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
			new Buffer(device, sizeof(Uniforms::Compute),
			           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	}
}

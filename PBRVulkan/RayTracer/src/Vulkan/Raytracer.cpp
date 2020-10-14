#include "Raytracer.h"

#include "BLAS.h"
#include "TLAS.h"
#include "Image.h"
#include "Device.h"
#include "Command.cpp"
#include "Buffer.h"
#include "SwapChain.h"
#include "ImageView.h"
#include "Extensions.h"
#include "ShaderBindingTable.h"
#include "RaytracerGraphicsPipeline.h"

#include "../Tracer/Scene.h"

namespace Vulkan
{
	static void ImageMemoryBarrier(
		const VkCommandBuffer commandBuffer,
		const VkImage image,
		const VkImageSubresourceRange subresourceRange,
		const VkAccessFlags srcAccessMask,
		const VkAccessFlags dstAccessMask,
		const VkImageLayout oldLayout,
		const VkImageLayout newLayout)
	{
		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange = subresourceRange;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		                     VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1,
		                     &barrier);
	}

	Raytracer::Raytracer()
	{
		extensions.reset(new Extensions(*device));
	}

	Raytracer::~Raytracer()
	{
		Raytracer::DeleteSwapChain();
	}

	void Raytracer::CreateSwapChain()
	{
		Rasterizer::CreateSwapChain();
		CreateOutputTexture();

		raytracerGraphicsPipeline.reset(
			new RaytracerGraphicsPipeline(
				*swapChain,
				*device,
				*scene,
				*accumulationImageView,
				*outputImageView,
				uniformBuffers,
				TLASs[0].Get()));

		shaderBindingTable.reset(new ShaderBindingTable(*raytracerGraphicsPipeline));
	}

	void Raytracer::DeleteSwapChain() {}

	void Raytracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		const auto extent = swapChain->Extent;

		VkDescriptorSet descriptorSets[] = { raytracerGraphicsPipeline->GetDescriptorsSets()[imageIndex] };

		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		ImageMemoryBarrier(commandBuffer, accumulationImage->Get(), subresourceRange, 0,
		                   VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		ImageMemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange, 0,
		                   VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV,
		                  raytracerGraphicsPipeline->GetPipeline());
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV,
		                        raytracerGraphicsPipeline->GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

		extensions->vkCmdTraceRaysNV(
			commandBuffer,
			shaderBindingTable->GetBuffer().Get(), 0,
			shaderBindingTable->GetBuffer().Get(), shaderBindingTable->GetEntrySize(),
			shaderBindingTable->GetEntrySize(),
			shaderBindingTable->GetBuffer().Get(), shaderBindingTable->GetEntrySize() * 2,
			shaderBindingTable->GetEntrySize(),
			nullptr, 0, 0,
			extent.width, extent.height, 1);

		ImageMemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange,
		                   VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
		                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		ImageMemoryBarrier(commandBuffer, swapChain->GetImage()[imageIndex], subresourceRange, 0,
		                   VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkImageCopy copyRegion;
		copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copyRegion.srcOffset = { 0, 0, 0 };
		copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copyRegion.dstOffset = { 0, 0, 0 };
		copyRegion.extent = { extent.width, extent.height, 1 };

		vkCmdCopyImage(commandBuffer,
		               outputImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		               swapChain->GetImage()[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		               1, &copyRegion);

		ImageMemoryBarrier(commandBuffer, swapChain->GetImage()[imageIndex], subresourceRange,
		                   VK_ACCESS_TRANSFER_WRITE_BIT,
		                   0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	void Raytracer::CreateOutputTexture()
	{
		const auto extent = swapChain->Extent;
		const auto outputFormat = swapChain->Format;
		const auto accumulationFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		const auto tiling = VK_IMAGE_TILING_OPTIMAL;

		accumulationImage.reset(
			new Image(*device, extent, accumulationFormat, tiling, VK_IMAGE_USAGE_STORAGE_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		outputImage.reset(
			new Image(*device, extent, outputFormat, tiling,
			          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		accumulationImageView.reset(new ImageView(*device, accumulationImage->Get(), accumulationFormat));
		outputImageView.reset(new ImageView(*device, outputImage->Get(), outputFormat));
	}

	void Raytracer::CreateAS()
	{
		Command::Submit(*commandPool, [this](VkCommandBuffer commandBuffer)
		{
			CreateBLAS(commandBuffer);
			AccelerationStructure::MemoryBarrier(commandBuffer);
			CreateTLAS(commandBuffer);
		});
	}

	void Raytracer::CreateBLAS(VkCommandBuffer commandBuffer)
	{
		uint32_t vertexOffset = 0;
		uint32_t indexOffset = 0;

		std::vector<ASMemoryRequirements> requirements;

		for (const auto& model : scene->GetMeshes())
		{
			const auto vertexCount = static_cast<uint32_t>(model->GetVerticesSize());
			const auto indexCount = static_cast<uint32_t>(model->GetIndeciesSize());
			const std::vector<VkGeometryNV> geometries =
			{
				BLAS::CreateGeometry(*scene, vertexOffset, vertexCount, indexOffset, indexCount, true)
			};

			BLASs.emplace_back(*device, geometries, false);
			requirements.push_back(BLASs.back().GetMemoryRequirements());

			vertexOffset += vertexCount * sizeof(Geometry::Vertex);
			indexOffset += indexCount * sizeof(uint32_t);
		}

		// Allocate the structure memory.
		const auto total = AccelerationStructure::GetTotalRequirements(requirements);

		BLASBuffer.reset(new Buffer(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		const auto BLASScratchBuffer = std::make_unique<Buffer>(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Generate the structures.
		VkDeviceSize resultOffset = 0;
		VkDeviceSize scratchOffset = 0;

		for (size_t i = 0; i != BLASs.size(); ++i)
		{
			BLASs[i].Generate(commandBuffer, *BLASScratchBuffer, scratchOffset, *BLASBuffer, resultOffset, false);
			resultOffset += requirements[i].Result.size;
			scratchOffset += requirements[i].Build.size;
		}
	}

	void Raytracer::CreateTLAS(VkCommandBuffer commandBuffer)
	{
		std::vector<VkGeometryInstance> geometryInstances;
		std::vector<ASMemoryRequirements> requirements;

		geometryInstances.reserve(scene->GetMeshes().size());
		for (auto instanceId = 0; instanceId < scene->GetMeshes().size(); ++instanceId)
		{
			geometryInstances.push_back(TLAS::CreateGeometryInstance(BLASs[instanceId], glm::mat4(1), instanceId));
		}

		TLASs.emplace_back(*device, geometryInstances, false);
		requirements.push_back(TLASs.back().GetMemoryRequirements());

		const auto total = AccelerationStructure::GetTotalRequirements(requirements);
		const size_t instancesBufferSize = sizeof(VkGeometryInstance) * geometryInstances.size();

		TLASBuffer.reset(new Buffer(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		const auto TLASScratchBuffer = std::make_unique<Buffer>(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		instanceBuffer.reset(
			new Buffer(*device, instancesBufferSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV,
			           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		TLASs.front().Generate(commandBuffer, *TLASScratchBuffer, 0, *TLASBuffer, 0, *instanceBuffer, 0, false);
	}
}

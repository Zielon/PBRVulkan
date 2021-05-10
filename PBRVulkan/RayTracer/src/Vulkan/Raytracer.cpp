#include "Raytracer.h"

#include <chrono>
#include <iostream>

#include "BLAS.h"
#include "TLAS.h"
#include "Image.h"
#include "Command.cpp"
#include "Buffer.h"
#include "SwapChain.h"
#include "ImageView.h"
#include "Extensions.h"
#include "ShaderBindingTable.h"
#include "RaytracerGraphicsPipeline.h"

#include "../Assets/Mesh.h"
#include "../Tracer/Scene.h"

namespace Vulkan
{
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
		CreateGraphicsPipeline();
	}

	void Raytracer::DeleteSwapChain()
	{
		scene.reset();
		raytracerGraphicsPipeline.reset();
		shaderBindingTable.reset();

		BLASs.clear();
		TLASs.clear();

		std::cout << "[RAYTRACER] Swap chain has been deleted." << std::endl;

		Rasterizer::DeleteSwapChain();
	}

	void Raytracer::CreateGraphicsPipeline()
	{
		raytracerGraphicsPipeline.reset(
			new RaytracerGraphicsPipeline(
				*swapChain,
				*device,
				*scene,
				*accumulationImageView,
				*outputImageView,
				*normalsImageView,
				*positionsImageView,
				uniformBuffers,
				// For now assume only one instance of Top Level Instance
				TLASs.front().Get()));

		shaderBindingTable.reset(new ShaderBindingTable(*raytracerGraphicsPipeline));
	}

	void Raytracer::Clear(VkCommandBuffer commandBuffer) const
	{
		VkImageSubresourceRange subresourceRange = Image::GetSubresourceRange();

		Image::MemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
		                     VK_IMAGE_LAYOUT_UNDEFINED,
		                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkClearColorValue color = Image::GetColor(0, 0, 0);

		vkCmdClearColorImage(commandBuffer, outputImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1,
		                     &subresourceRange);

		Image::MemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange,
		                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	void Raytracer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		const auto extent = swapChain->Extent;

		VkDescriptorSet descriptorSets[] = { raytracerGraphicsPipeline->GetDescriptorsSets()[imageIndex] };

		VkImageSubresourceRange subresourceRange = Image::GetSubresourceRange();

		Image::MemoryBarrier(commandBuffer, accumulationImage->Get(), subresourceRange, 0,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		Image::MemoryBarrier(commandBuffer, outputImage->Get(), subresourceRange, 0,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		Image::MemoryBarrier(commandBuffer, normalsImage->Get(), subresourceRange, 0,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		Image::MemoryBarrier(commandBuffer, positionsImage->Get(), subresourceRange, 0,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV,
		                  raytracerGraphicsPipeline->GetPipeline());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV,
		                        raytracerGraphicsPipeline->GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

		extensions->vkCmdTraceRaysNV(commandBuffer,
		                             shaderBindingTable->GetBuffer().Get(), size_t(0),
		                             shaderBindingTable->GetBuffer().Get(), shaderBindingTable->GetEntrySize(),
		                             shaderBindingTable->GetEntrySize(),
		                             shaderBindingTable->GetBuffer().Get(), shaderBindingTable->GetEntrySize() * 3,
		                             shaderBindingTable->GetEntrySize(),
		                             nullptr, 0, 0,
		                             extent.width, extent.height, 1);

		// Do not copy image to swap chain
		if (settings.UseComputeShaders)
			return;

		Copy(commandBuffer, outputImage->Get(), swapChain->GetImage()[imageIndex]);
	}

	void Raytracer::CreateOutputTexture()
	{
		const auto extent = swapChain->Extent;
		const auto outputFormat = swapChain->Format;
		const auto accumulationFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		const auto tiling = VK_IMAGE_TILING_OPTIMAL;

		accumulationImage.reset(
			new Image(*device, extent, accumulationFormat, tiling, VK_IMAGE_TYPE_2D,
			          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		outputImage.reset(
			new Image(*device, extent, outputFormat, tiling, VK_IMAGE_TYPE_2D,
			          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		normalsImage.reset(
			new Image(*device, extent, accumulationFormat, tiling, VK_IMAGE_TYPE_2D, VK_IMAGE_USAGE_STORAGE_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		positionsImage.reset(
			new Image(*device, extent, accumulationFormat, tiling, VK_IMAGE_TYPE_2D, VK_IMAGE_USAGE_STORAGE_BIT,
			          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		accumulationImageView.reset(new ImageView(*device, accumulationImage->Get(), accumulationFormat));
		outputImageView.reset(new ImageView(*device, outputImage->Get(), outputFormat));
		normalsImageView.reset(new ImageView(*device, normalsImage->Get(), accumulationFormat));
		positionsImageView.reset(new ImageView(*device, positionsImage->Get(), accumulationFormat));
	}

	void Raytracer::CreateAS()
	{
		const auto start = std::chrono::high_resolution_clock::now();

		Command::Submit(*commandPool, [this](VkCommandBuffer commandBuffer)
		{
			CreateBLAS(commandBuffer);
			AccelerationStructure::MemoryBarrier(commandBuffer);
			CreateTLAS(commandBuffer);
		});

		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		std::cout << "[RAYTRACER] Acceleration data structure build: " << duration.count() << " milliseconds" <<
			std::endl;
	}

	void Raytracer::CreateBLAS(VkCommandBuffer commandBuffer)
	{
		uint32_t vertexOffset = 0;
		uint32_t indexOffset = 0;

		for (const auto& model : scene->GetMeshInstances())
		{
			const auto& mesh = scene->GetMeshes()[model.meshId];

			const auto vertexCount = static_cast<uint32_t>(mesh->GetVerticesSize());
			const auto indexCount = static_cast<uint32_t>(mesh->GetIndeciesSize());

			VkAccelerationStructureGeometryKHR geometry = 
				BLAS::CreateGeometry(*scene, vertexOffset, vertexCount, indexOffset, indexCount, true);

			BLASs.emplace_back(*device, geometry);

			vertexOffset += vertexCount * sizeof(Geometry::Vertex);
			indexOffset += indexCount * sizeof(uint32_t);
		}

		// Allocate the structure memory.
		const auto total = AccelerationStructure::ReduceMemory(BLASs);

		BLASBuffer.reset(new Buffer(
			*device, total.accelerationStructureSize, 
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		ScratchBLASBuffer.reset(new Buffer(
			*device, total.buildScratchSize, 
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		// Generate the structures.
		VkDeviceSize resultOffset = 0;
		VkDeviceSize scratchOffset = 0;

		for (size_t i = 0; i < BLASs.size(); ++i)
		{
			BLASs[i].Generate(commandBuffer, *ScratchBLASBuffer, scratchOffset, *BLASBuffer, resultOffset);
			resultOffset += BLASs[i].buildSizesInfo.accelerationStructureSize;
			scratchOffset += BLASs[i].buildSizesInfo.buildScratchSize;
		}
	}

	void Raytracer::CreateTLAS(VkCommandBuffer commandBuffer)
	{
		std::vector<VkAccelerationStructureInstanceKHR> geometryInstances;

		geometryInstances.reserve(scene->GetMeshes().size());

		for (auto instanceId = 0; instanceId < scene->GetMeshInstances().size(); ++instanceId)
		{
			geometryInstances.push_back(TLAS::CreateGeometryInstance(BLASs[instanceId], glm::mat4(1), instanceId));
		}

		TLASs.emplace_back(*device, geometryInstances);

		const auto total = AccelerationStructure::ReduceMemory(TLASs);

		AccelerationStructure::MemoryBarrier(commandBuffer);

		TLASBuffer.reset(new Buffer(
			*device, total.accelerationStructureSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		ScratchTLASBuffer.reset(new Buffer(
			*device, total.buildScratchSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		instanceBuffer.reset(
			new Buffer(*device, instancesBufferSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV,
			           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		// For now assume only one instance of Top Level Instance
		TLASs.front().Generate(commandBuffer, *ScratchTLASBuffer, 0, *TLASBuffer, 0, *instanceBuffer, 0);
	}
}

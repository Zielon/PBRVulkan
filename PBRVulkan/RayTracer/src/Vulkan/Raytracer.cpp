#include "Raytracer.h"

#include "BLAS.h"
#include "TLAS.h"
#include "Image.h"
#include "Command.cpp"
#include "Buffer.h"
#include "SwapChain.h"
#include "ImageView.h"
#include "ShaderBindingTable.h"
#include "RaytracerGraphicsPipeline.h"

#include "../Tracer/Scene.h"

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
		// TODO
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

		const auto blasScratchBuffer = std::make_unique<Buffer>(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Generate the structures.
		VkDeviceSize resultOffset = 0;
		VkDeviceSize scratchOffset = 0;

		for (size_t i = 0; i != BLASs.size(); ++i)
		{
			BLASs[i].Generate(commandBuffer, *blasScratchBuffer, scratchOffset, *BLASBuffer, resultOffset, false);
			resultOffset += requirements[i].Result.size;
			scratchOffset += requirements[i].Build.size;
		}
	}

	void Raytracer::CreateTLAS(VkCommandBuffer commandBuffer)
	{
		std::vector<VkGeometryInstance> geometryInstances;
		std::vector<ASMemoryRequirements> requirements;

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
		const auto tlasScratchBuffer = std::make_unique<Buffer>(
			*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		instanceBuffer.reset(
			new Buffer(*device, total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV,
			           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		TLASs.front().Generate(commandBuffer, *tlasScratchBuffer, 0, *TLASBuffer, 0, *instanceBuffer, 0, false);
	}
}

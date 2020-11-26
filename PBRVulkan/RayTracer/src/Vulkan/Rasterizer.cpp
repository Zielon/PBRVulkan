#include "Rasterizer.h"

#include <array>
#include <iostream>

#include "../Assets/Mesh.h"
#include "../Tracer/Scene.h"

#include "Buffer.h"
#include "DepthBuffer.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "SwapChain.h"
#include "RasterizerGraphicsPipeline.h"
#include "CommandBuffers.h"
#include "Semaphore.h"

#include "../Geometry/Global.h"

namespace Vulkan
{
	Rasterizer::Rasterizer() { }

	Rasterizer::~Rasterizer()
	{
		Rasterizer::DeleteSwapChain();
	}

	void Rasterizer::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rasterizerGraphicsPipeline->GetRenderPass();
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->Extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkBuffer vertexBuffers[] = { scene->GetVertexBuffer().Get() };
		const VkBuffer indexBuffer = scene->GetIndexBuffer().Get();
		VkDescriptorSet descriptorSets[] = { rasterizerGraphicsPipeline->GetDescriptorSets()[imageIndex] };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                  rasterizerGraphicsPipeline->GetPipeline());
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                        rasterizerGraphicsPipeline->GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

			uint32_t vertexOffset = 0;
			uint32_t indexOffset = 0;

			// Render all models with proper buffers offsets
			for (const auto& mesh : scene->GetMeshInstances())
			{
				const uint32_t indecies = scene->GetMeshes()[mesh.meshId]->GetIndeciesSize();

				vkCmdDrawIndexed(commandBuffer, indecies, 1, indexOffset, vertexOffset, 0);

				vertexOffset += scene->GetMeshes()[mesh.meshId]->GetVerticesSize();
				indexOffset += indecies;
			}
		}
		vkCmdEndRenderPass(commandBuffer);
	}

	void Rasterizer::CreateSwapChain()
	{
		if (!scene)
			throw std::runtime_error("Scene has not been loaded!");

		swapChain.reset(new SwapChain(*device));
		depthBuffer.reset(new DepthBuffer(*commandPool, swapChain->Extent));

		for (const auto& _ : swapChain->GetImageViews())
		{
			inFlightFences.emplace_back(new Fence(*device));
			imageAvailableSemaphores.emplace_back(new Semaphore(*device));
			renderFinishedSemaphores.emplace_back(new Semaphore(*device));
			uniformBuffers.emplace_back(
				new Buffer(*device, sizeof(Uniforms::Global),
				           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		}

		Rasterizer::CreateGraphicsPipeline();
	}

	void Rasterizer::DeleteSwapChain()
	{
		commandBuffers.reset();
		swapChainFrameBuffers.clear();
		rasterizerGraphicsPipeline.reset();
		uniformBuffers.clear();
		inFlightFences.clear();
		renderFinishedSemaphores.clear();
		imageAvailableSemaphores.clear();
		depthBuffer.reset();

		if (swapChain)
		{
			swapChain.reset();
			std::cout << "[RASTERIZER] Swap chain has been deleted." << std::endl;
		}
	}

	void Rasterizer::CreateGraphicsPipeline()
	{
		if (!scene)
			throw std::runtime_error("Scene has not been loaded!");

		swapChainFrameBuffers.clear();

		rasterizerGraphicsPipeline.reset(
			new RasterizerGraphicsPipeline(*swapChain, *device, *scene, uniformBuffers));

		for (const auto& imageView : swapChain->GetImageViews())
		{
			swapChainFrameBuffers.emplace_back(
				new Framebuffer(*imageView, *swapChain, *depthBuffer, rasterizerGraphicsPipeline->GetRenderPass()));
		}

		commandBuffers.reset(new CommandBuffers(*commandPool, static_cast<uint32_t>(swapChainFrameBuffers.size())));
	}

	void Rasterizer::Copy(VkCommandBuffer commandBuffer, VkImage src, VkImage dst) const
	{
		VkExtent2D extent = swapChain->Extent;
		VkImageSubresourceRange subresourceRange = Image::GetSubresourceRange();
		VkImageCopy copyRegion = Image::GetImageCopy(extent.width, extent.height);

		Image::MemoryBarrier(commandBuffer, src, subresourceRange,
		                     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
		                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		Image::MemoryBarrier(commandBuffer, dst, subresourceRange, 0,
		                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		vkCmdCopyImage(commandBuffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst,
		               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		Image::MemoryBarrier(commandBuffer, dst, subresourceRange,
		                     VK_ACCESS_TRANSFER_WRITE_BIT, 0,
		                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL);
	}
}

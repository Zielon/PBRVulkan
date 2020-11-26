#pragma once

#include "Vulkan.h"

#include "Rasterizer.h"
#include "RaytracerGraphicsPipeline.h"

namespace Vulkan
{
	/*
	 * Raytracer assumes that swap chain is already initialized by rasterizer.
	 */
	class Raytracer : public Rasterizer
	{
	public:
		NON_COPIABLE(Raytracer)

		Raytracer();
		~Raytracer();

		void CreateSwapChain() override;
		void DeleteSwapChain() override;
		void CreateGraphicsPipeline() override;
		void Clear(VkCommandBuffer commandBuffer) const;

		[[nodiscard]] class ImageView& GetOutputImageView() const
		{
			return *outputImageView;
		}

		[[nodiscard]] class ImageView& GetNormalsImageView() const
		{
			return *normalsImageView;
		}

		[[nodiscard]] class ImageView& GetPositionImageView() const
		{
			return *positionsImageView;
		}

		[[nodiscard]] const class Image& GetOutputImage() const
		{
			return *outputImage;
		}

	protected:
		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex) override;
		void CreateOutputTexture();
		void CreateAS();

	private:
		void CreateBLAS(VkCommandBuffer commandBuffer);
		void CreateTLAS(VkCommandBuffer commandBuffer);

		std::vector<class TLAS> TLASs;
		std::vector<class BLAS> BLASs;

		std::unique_ptr<class Image> accumulationImage;
		std::unique_ptr<class ImageView> accumulationImageView;
		std::unique_ptr<class Image> outputImage;
		std::unique_ptr<class ImageView> outputImageView;
		std::unique_ptr<class Image> normalsImage;
		std::unique_ptr<class ImageView> normalsImageView;
		std::unique_ptr<class Image> positionsImage;
		std::unique_ptr<class ImageView> positionsImageView;

		std::unique_ptr<class Buffer> instanceBuffer;
		std::unique_ptr<class Buffer> BLASBuffer;
		std::unique_ptr<class Buffer> ScratchBLASBuffer;
		std::unique_ptr<class Buffer> TLASBuffer;
		std::unique_ptr<class Buffer> ScratchTLASBuffer;
		std::unique_ptr<class Extensions> extensions;
		std::unique_ptr<class ShaderBindingTable> shaderBindingTable;
		std::unique_ptr<class RaytracerGraphicsPipeline> raytracerGraphicsPipeline;
	};
}

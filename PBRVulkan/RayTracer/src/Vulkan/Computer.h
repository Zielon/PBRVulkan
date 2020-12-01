#pragma once

#include <memory>
#include <vector>


#include "Vulkan.h"

namespace Vulkan
{
	class Computer final
	{
	public:
		NON_COPIABLE(Computer)

		Computer(
			const class SwapChain& swapChain,
			const class Device& device,
			const class ImageView& inputImageView,
			const class ImageView& normalsImageView,
			const class ImageView& positionsImageView);
		~Computer();

		void BuildCommand(int32_t shaderId);

		[[nodiscard]] const class Image& GetOutputImage() const
		{
			return *outputImage;
		}

		[[nodiscard]] const class CommandBuffers& GetCommandBuffers() const
		{
			return *commandBuffers;
		}

		[[nodiscard]] const class Semaphore& GetSemaphore() const
		{
			return *semaphore;
		}

	private:
		void CreateOutputTexture();
		void CreateUniformBuffer();

		int32_t currentShader = -1;

		const SwapChain& swapChain;
		const Device& device;

		const ImageView& inputImageView;
		const ImageView& normalsImageView;
		const ImageView& positionsImageView;

		std::unique_ptr<class Image> outputImage;
		std::unique_ptr<class ImageView> outputImageView;
		std::vector<std::unique_ptr<class Buffer>> uniformBuffers;

		std::unique_ptr<class CommandPool> commandPool;
		std::unique_ptr<class CommandBuffers> commandBuffers;
		std::unique_ptr<class ComputePipeline> computePipeline;
		std::unique_ptr<class Semaphore> semaphore;
	};
}

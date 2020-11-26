#pragma once

#include "Core.h"

namespace Vulkan
{
	class Rasterizer : public Core
	{
	public:
		NON_COPIABLE(Rasterizer)

		Rasterizer();
		~Rasterizer();

		void CreateSwapChain() override;
		void DeleteSwapChain() override;
		void CreateGraphicsPipeline() override;
		void Copy(VkCommandBuffer commandBuffer, VkImage src, VkImage dst) const;
		
	protected:
		void Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

	private:
		std::unique_ptr<class RasterizerGraphicsPipeline> rasterizerGraphicsPipeline;
	};
}

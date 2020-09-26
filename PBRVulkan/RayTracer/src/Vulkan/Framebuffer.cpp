#include "Framebuffer.h"

#include "ImageView.h"
#include "SwapChain.h"
#include "Device.h"
#include "DepthBuffer.h"

#include <array>

namespace Vulkan
{
	Framebuffer::Framebuffer(
		const ImageView& imageView,
		const SwapChain& swapChain,
		const DepthBuffer& depthBuffer,
		VkRenderPass renderPass): imageView(imageView)
	{
		std::array<VkImageView, 2> attachments =
		{
			imageView.Get(),
			depthBuffer.GetImageView().Get()
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChain.Extent.width;
		framebufferInfo.height = swapChain.Extent.height;
		framebufferInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(imageView.GetDevice().Get(), &framebufferInfo, nullptr, &framebuffer),
		         "Create framebuffer");
	}

	Framebuffer::~Framebuffer()
	{
		if (framebuffer != nullptr)
		{
			vkDestroyFramebuffer(imageView.GetDevice().Get(), framebuffer, nullptr);
			framebuffer = nullptr;
		}
	}
}

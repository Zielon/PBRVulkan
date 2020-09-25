#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class TextureSampler final
	{
	public:
		NON_COPIABLE(TextureSampler)

		TextureSampler(const class Device& device);
		~TextureSampler();

		[[nodiscard]] VkSampler Get() const
		{
			return sampler;
		}

		[[nodiscard]] const Device& GetDevice() const
		{
			return device;
		}

	private:
		const Device& device;
		VkSampler sampler{};
	};
}

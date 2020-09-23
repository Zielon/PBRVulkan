#pragma once

#include <memory>

#include "Vulkan.h"

namespace Vulkan
{
	class DescriptorSetLayout final
	{
	public:
		NON_COPIABLE(DescriptorSetLayout)

		DescriptorSetLayout(const class Device& device);
		~DescriptorSetLayout();

		[[nodiscard]] VkDescriptorSetLayout Get() const
		{
			return descriptorSetLayout;
		}

	private:
		const class Device& device;
		VkDescriptorSetLayout descriptorSetLayout{};
	};
}

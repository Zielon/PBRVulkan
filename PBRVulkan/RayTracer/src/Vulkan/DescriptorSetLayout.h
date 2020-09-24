#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	/**
	 * The descriptor layout specifies the types of resources that
	 * are going to be accessed by the pipeline.
	 */
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

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

	private:
		const class Device& device;
		VkDescriptorSetLayout descriptorSetLayout{};
	};
}

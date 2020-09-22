#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Device;
	
	class Buffer final
	{
	public:		
		NON_COPIABLE(Buffer)

		Buffer(const Vulkan::Device& device);
		~Buffer();

		[[nodiscard]] const VkBuffer& Get() const
		{
			return buffer;
		}

	private:
		VkBuffer buffer;
	};
}

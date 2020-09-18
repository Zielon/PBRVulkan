#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Fence final
	{
	public:
		NON_COPIABLE(Fence)

		Fence(const class Device& device);
		~Fence();

		[[nodiscard]] const class Device& GetDevice() const { return device; }
		[[nodiscard]] const VkFence& Get() const { return fence; }
		void Reset() const;
		void Wait(uint64_t timeout) const;

	private:
		const class Device& device;
		VkFence fence{};
	};
}

#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Instance;

	class Surface final
	{
	public:
		NON_COPIABLE(Surface)

		explicit Surface(const Instance& instance);
		~Surface();

		[[nodiscard]] VkSurfaceKHR Get() const
		{
			return surface;
		};

		[[nodiscard]] const Instance& GetInstance() const
		{
			return instance;
		}

	private:
		VkSurfaceKHR surface{};
		const Instance& instance;
	};
}

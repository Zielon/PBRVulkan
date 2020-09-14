#pragma once

#include "Vulkan.h"

#include <vector>
#include <memory>

namespace Vulkan
{
	class Application
	{
	public:
		NON_COPIABLE(Application)

		explicit Application();
		virtual ~Application();

		void Run();

	private:
		std::unique_ptr<class Window> window;
	};
}
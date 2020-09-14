#pragma once

#include "../Vulkan/Vulkan.h"

namespace Tracer 
{
	class Menu 
	{
	public:
		NON_COPIABLE(Menu)

		Menu();
		~Menu();

		void Render();
	};
}
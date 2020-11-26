#pragma once

#include "../Settings.h"

namespace Interface
{
	class Widget
	{
	public:
		virtual ~Widget() = default;
		virtual void Render(Settings& settings) = 0;
	};
}

#pragma once

#include "Widget.h"

namespace Interface
{
	class CinemaWidget final : public Widget
	{
	public:
		~CinemaWidget() = default;
		
		void Render(Settings& settings) override;
	};
}

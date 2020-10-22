#pragma once

#include "Widget.h"

namespace Interface
{
	class RendererWidget final : public Widget
	{
	public:
		~RendererWidget() = default;
		
		void Render(Tracer::Settings& settings) override;
	};
}

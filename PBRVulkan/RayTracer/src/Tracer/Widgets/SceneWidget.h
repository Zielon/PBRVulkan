#pragma once

#include "Widget.h"

namespace Interface
{
	class SceneWidget final : public Widget
	{
	public:
		~SceneWidget() = default;
		
		void Render(Tracer::Settings& settings) override;
	};
}

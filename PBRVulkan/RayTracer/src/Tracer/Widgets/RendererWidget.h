#pragma once

#include "Widget.h"

namespace Interface
{
	class RendererWidget final : public Widget
	{
	public:
		~RendererWidget() = default;

		void Render(Tracer::Settings& settings) override;

	private:
		const char* computeShaders[3] = {
			"Denoiser",
			"Edge detection",
			"Sharpen"
		};
	};
}

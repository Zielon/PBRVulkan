#pragma once

#include "Widget.h"

namespace Interface
{
	class SceneWidget final : public Widget
	{
	public:
		~SceneWidget() = default;

		void Render(Tracer::Settings& settings) override;

	private:
		const char* integrators[3] = {
			"Path tracer",
			"SMS",
			"AO"
		};

		const char* scenes[14] = {
			"Ajax",
			"Bedroom",
			"Boy",
			"Coffee cart",
			"Coffee maker",
			"Cornell box",
			"Dining room",
			"Dragon",
			"Hyperion",
			"Panther",
			"Spaceship",
			"Staircase",
			"Stormtroopers",
			"Teapot"
		};
	};
}

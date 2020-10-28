#include "SceneWidget.h"

#include <imgui.h>

#include "../Menu.h"

namespace Interface
{
	void SceneWidget::Render(Tracer::Settings& settings)
	{
		const char* integrators[] = { "Path tracer", "MSM", "AO" };

		const char* scenes[] = {
			"Coffee cart",
			"Cornell box",
			"Ajax",
			"Bedroom",
			"Staircase",
			"Dining room",
			"Dragon",
			"Spaceship",
			"Stormtrooper"
		};

		ImGui::Text("Shaders");

		ImGui::PushItemWidth(-1);
		ImGui::Combo(" ", &settings.IntegratorType, integrators, 3);
		ImGui::PopItemWidth();

		ImGui::Text("Scene");

		ImGui::PushItemWidth(-1);
		ImGui::Combo("  ", &settings.SceneId, scenes, 9);
		ImGui::PopItemWidth();
	}
}

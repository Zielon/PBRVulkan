#include "SceneWidget.h"

#include <imgui.h>

namespace Interface
{	
	void SceneWidget::Render(Settings& settings)
	{
		ImGui::Text("Shaders");

		ImGui::PushItemWidth(-1);
		ImGui::Combo("integrators", &settings.IntegratorType, integrators, 3);
		ImGui::PopItemWidth();

		if (settings.IntegratorType == 2)
		{
			ImGui::Text("AO ray length");
			ImGui::SameLine();
			ImGui::InputFloat("AO_samples", &settings.AORayLength, 0.01);
		}

		ImGui::Text("Scene");

		ImGui::PushItemWidth(-1);
		ImGui::Combo("  ", &settings.SceneId, scenes, CONFIGS.size());
		ImGui::PopItemWidth();
	}
}

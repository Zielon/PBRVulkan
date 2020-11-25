#include "RendererWidget.h"

#include <imgui.h>

#include "../Menu.h"

namespace Interface
{
	void RendererWidget::Render(Tracer::Settings& settings)
	{
		ImGui::Text("Renderer");
		ImGui::Separator();

		ImGui::Checkbox("Use rasterizer", &settings.UseRasterizer);
		ImGui::Checkbox("Gamma correction", &settings.UseGammaCorrection);
		ImGui::Checkbox("Compute shaders", &settings.UseComputeShaders);

		if (settings.UseComputeShaders)
		{
			ImGui::PushItemWidth(-1);
			ImGui::Combo("compute shaders", &settings.ComputeShaderId, computeShaders, 3);
			ImGui::PopItemWidth();
		}

		ImGui::Text("# samples ");
		ImGui::SameLine();
		ImGui::InputInt("int_samples", &settings.SSP, 1);

		ImGui::Text("# depth   ");
		ImGui::SameLine();
		ImGui::InputInt("int_depth", &settings.MaxDepth, 1);

		ImGui::Text("Focal     ");
		ImGui::SameLine();
		ImGui::InputFloat("float_focal", &settings.FocalDistance, 0.1);

		ImGui::Text("Aperture  ");
		ImGui::SameLine();
		ImGui::InputFloat("float_aperture", &settings.Aperture, 0.1);
	}
}

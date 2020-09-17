#include "Menu.h"

#include "imgui.h"

#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_vulkan.h"

namespace Tracer
{
	Menu::Menu()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	Menu::~Menu()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Menu::Render()
	{
	}
}

#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>

#include "Menu.h"
#include "Scene.h"
#include "Camera.h"
#include "Compiler.h"

#include "../Geometry/Global.h"
#include "../Vulkan/Window.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/Device.h"

#include "Widgets/CinemaWidget.h"
#include "Widgets/RendererWidget.h"
#include "Widgets/SceneWidget.h"

namespace Tracer
{
	std::vector<std::string> CONFIGS =
	{
		"../Assets/Scenes/coffee_cart.scene",
		"../Assets/Scenes/cornell_box.scene",
		"../Assets/Scenes/ajax.scene",
		"../Assets/Scenes/bedroom.scene",
		"../Assets/Scenes/staircase.scene",
		"../Assets/Scenes/diningroom.scene",
		"../Assets/Scenes/dragon.scene",
		"../Assets/Scenes/spaceship.scene",
		"../Assets/Scenes/stormtrooper.scene"
	};

	Application::Application()
	{
		LoadScene();
		compiler.reset(new Compiler());
		CompileShaders();
		CreateAS();
		RegisterCallbacks();
		Raytracer::CreateSwapChain();
		CreateMenu();
	}

	void Application::LoadScene()
	{
		scene.reset(new Scene(CONFIGS[settings.SceneId], *device, *commandPool));
	}

	void Application::UpdateSettings()
	{
		if (settings.SceneId != menu->GetSettings().SceneId)
			RecreateSwapChain();
		
		if (settings.RequiresShaderRecompliation(menu->GetSettings()))
			RecompileShaders();

		if (settings.RequiresAccumulationReset(menu->GetSettings()))
			ResetAccumulation();

		settings = menu->GetSettings();
	}

	void Application::CompileShaders() const
	{
		std::vector<Parser::Define> defines;
		std::vector<Parser::Include> includes;

		if (scene->UseHDR())
			defines.push_back(Parser::Define::USE_HDR);

		if(settings.UseGammaCorrection)
			defines.push_back(Parser::Define::USE_GAMMA_CORRECTION);

		includes.push_back(static_cast<Parser::Include>(settings.IntegratorType));

		compiler->Compile(includes, defines);
	}

	void Application::RecreateSwapChain()
	{
		settings = menu->GetSettings();
		device->WaitIdle();
		menu.reset();
		scene.reset();
		Raytracer::DeleteSwapChain();
		LoadScene();
		CompileShaders();
		CreateAS();
		Raytracer::CreateSwapChain();
		CreateMenu();
		ResetAccumulation();
	}

	void Application::RecompileShaders()
	{
		settings = menu->GetSettings();
		device->WaitIdle();
		CompileShaders();
		Raytracer::CreateGraphicsPipeline();
		ResetAccumulation();
	}

	void Application::CreateMenu()
	{
		menu.reset(new Menu(*device, *swapChain, *commandPool, settings));

		menu->AddWidget(std::make_shared<Interface::SceneWidget>());
		menu->AddWidget(std::make_shared<Interface::RendererWidget>());
		menu->AddWidget(std::make_shared<Interface::CinemaWidget>());
	}

	void Application::ResetAccumulation()
	{
		frame = 0;
	}

	void Application::UpdateUniformBuffer(uint32_t imageIndex)
	{
		Uniforms::Global uniform{};

		uniform.view = scene->GetCamera().GetView();
		uniform.projection = scene->GetCamera().GetProjection();
		uniform.direction = scene->GetCamera().GetDirection();
		uniform.random = glm::vec2(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
		uniform.lights = scene->GetLightsSize();
		uniform.hasHDR = scene->UseHDR();
		uniform.ssp = settings.SSP;
		uniform.maxDepth = settings.MaxDepth;
		uniform.aperture = settings.Aperture;
		uniform.focalDistance = settings.FocalDistance;
		uniform.hdrResolution = scene->UseHDR() ? scene->GetHDRResolution() : 0.f;
		uniform.frame = frame;
		uniform.AORayLength = settings.AORayLength;
	
		uniformBuffers[imageIndex]->Fill(&uniform);
	}

	void Application::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		Camera::TimeDeltaUpdate();

		if (scene->GetCamera().OnBeforeRender())
			ResetAccumulation();

		if (settings.UseRasterizer)
			Rasterizer::Render(framebuffer, commandBuffer, imageIndex);
		else
			Raytracer::Render(framebuffer, commandBuffer, imageIndex);

		menu->Render(framebuffer, commandBuffer);

		++frame;
	}

	void Application::RegisterCallbacks()
	{
		window->AddOnCursorPositionChanged([this](const double xpos, const double ypos)-> void
		{
			OnCursorPositionChanged(xpos, ypos);
		});

		window->AddOnKeyChanged([this](const int key, const int scancode, const int action, const int mods)-> void
		{
			OnKeyChanged(key, scancode, action, mods);
		});

		window->AddOnMouseButtonChanged([this](const int button, const int action, const int mods)-> void
		{
			OnMouseButtonChanged(button, action, mods);
		});

		window->AddOnScrollChanged([this](const double xoffset, const double yoffset)-> void
		{
			OnScrollChanged(xoffset, yoffset);
		});
	}

	void Application::OnKeyChanged(int key, int scanCode, int action, int mods)
	{
		if (menu->WantCaptureKeyboard() || !swapChain)
			return;

		scene->GetCamera().OnKeyChanged(key, scanCode, action, mods);
	}

	void Application::OnCursorPositionChanged(double xpos, double ypos)
	{
		if (menu->WantCaptureKeyboard() || menu->WantCaptureMouse() || !swapChain)
			return;

		if (scene->GetCamera().OnCursorPositionChanged(xpos, ypos))
			ResetAccumulation();
	}

	void Application::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (menu->WantCaptureMouse() || !swapChain)
			return;

		scene->GetCamera().OnMouseButtonChanged(button, action, mods);
	}

	void Application::OnScrollChanged(double xoffset, double yoffset)
	{
		if (menu->WantCaptureMouse())
			return;
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glfwPollEvents();
			UpdateSettings();
			DrawFrame();
		}

		device->WaitIdle();
	}
}

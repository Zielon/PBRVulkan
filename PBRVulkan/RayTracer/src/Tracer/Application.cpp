#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>

#include "Menu.h"
#include "Scene.h"
#include "Camera.h"

#include "../Geometry/MVP.h"

#include "Compiler.h"

#include "../Vulkan/Window.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/Device.h"

namespace Tracer
{
	//const std::string CONFIG = "../Assets/Scenes/cornell_box.scene";
	const std::string CONFIG = "../Assets/Scenes/coffee_cart.scene";

	Application::Application()
	{
		compiler.reset(new Compiler(PATH_TRACER_DEFAULT));

		LoadScene();
		CreateAS();
		RegisterCallbacks();
		Raytracer::CreateSwapChain();

		menu.reset(new Menu(*device, *swapChain, *commandPool));
	}

	Application::~Application() {}

	void Application::LoadScene()
	{
		scene.reset(new Scene(CONFIG, *device, *commandPool, RAYTRACER));
	}

	void Application::UpdatePipeline()
	{
		if (settings == menu->GetSettings())
			return;

		// Update scene
		// Recreate swap chain

		settings = menu->GetSettings();
	}

	void Application::UpdateUniformBuffer(uint32_t imageIndex)
	{
		Uniforms::MVP ubo{};

		ubo.view = scene->GetCamera().GetView();
		ubo.projection = scene->GetCamera().GetProjection();
		ubo.direction = scene->GetCamera().GetDirection();

		uniformBuffers[imageIndex]->Fill(&ubo);
	}

	void Application::Render(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		Camera::OnBeforeRender();
		UpdatePipeline();

		if (type == RAYTRACER)
			Raytracer::Render(framebuffer, commandBuffer, imageIndex);
		if (type == RASTERIZER)
			Rasterizer::Render(framebuffer, commandBuffer, imageIndex);

		scene->GetCamera().OnAfterRender();
		menu->Render(framebuffer, commandBuffer);
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

		scene->GetCamera().OnCursorPositionChanged(xpos, ypos);
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
			DrawFrame();
		}

		device->WaitIdle();
	}
}

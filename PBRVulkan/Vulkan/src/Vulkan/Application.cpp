#include "Application.h"
#include "Window.h"

namespace Vulkan 
{
	Application::Application()
	{
		window.reset(new Window());
	}

	Application::~Application()
	{
		window.reset();
	}

	void Application::Run()
	{
		window->Run();
	}
}
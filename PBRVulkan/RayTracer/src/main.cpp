#include "Tracer/Application.h"

int main()
{
	// Compile all shaders from Assets folder
	std::system("python ./Scripts/Compile.py");

	Tracer::Application application;

	application.Run();
}

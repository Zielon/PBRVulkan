#include "Tracer/RayTracer.h"

int main()
{
	// Compile all shaders from Assets folder
	std::system("python ./Scripts/Compile.py");

	Tracer::RayTracer tracer;
	tracer.Run();
}

#include "Compiler.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace Tracer
{
	namespace COMPILER
	{
		std::string TOKEN = "====== INTEGRATOR ======";
		std::string RAY_HIT_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rchit";

		std::map<Integrator, std::string> INTEGRATORS = {
			{ PATH_TRACER_DEFAULT, "#include \"Integrators/Default.glsl\"" }
		};
	}

	Compiler::Compiler(Integrator integrator): integrator(integrator) { }

	void Compiler::Compile()
	{
		std::cout << "[INFO] Shaders compilation has begin." << std::endl;

		Read();
		Include();
		std::system("python ./scripts/Compile.py");
		Restore();

		std::cout << "[INFO] Shaders compilation has ended." << std::endl;
	}

	void Compiler::Read()
	{
		std::ifstream inShader(COMPILER::RAY_HIT_SHADER);
		std::string line;
		int i = 0;
		while (std::getline(inShader, line))
		{
			file.push_back(line);
			if (line.find(COMPILER::TOKEN) != std::string::npos)
				tokenPosition = i;
			++i;
		}
		inShader.close();
	}

	void Compiler::Include()
	{
		std::ofstream outShader(COMPILER::RAY_HIT_SHADER);
		int i = 0;
		for (auto& line : file)
		{
			outShader << (i == tokenPosition ? COMPILER::INTEGRATORS[integrator] : line) << std::endl;
			++i;
		}
		outShader.close();
	}

	void Compiler::Restore()
	{
		std::ofstream outShader(COMPILER::RAY_HIT_SHADER);
		for (auto& line : file)
			outShader << line << std::endl;
		outShader.close();
	}
}

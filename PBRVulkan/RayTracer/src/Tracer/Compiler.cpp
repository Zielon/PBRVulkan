#include "Compiler.h"

#include <map>
#include <fstream>
#include <iostream>
#include <string>

namespace Tracer
{
	namespace COMPILER
	{
		enum Defines
		{
			USE_HDR
		};

		std::string TOKEN_INTEGRATOR = "// ====== INTEGRATOR ======";
		std::string TOKEN_DEFINES = "// ====== DEFINES ======";
		std::string RAY_HIT_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rchit";
		std::string RAY_MISS_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rmiss";
		std::string RAY_GEN_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rgen";

		std::map<Integrator, std::string> INTEGRATORS = {
			{ PATH_TRACER_DEFAULT, "#include \"Integrators/PathTracer.glsl\"" },
			{ PATH_TRACER_MSM, "#include \"Integrators/PathTracerMSM.glsl\"" }
		};
	}

	Compiler::Compiler(Integrator integrator, bool define): integrator(integrator)
	{
		std::cout << "[INFO] Shaders compilation has begin." << std::endl;

		Read();
		Include();
		if (define)
			Define();
		std::system("python ./scripts/Compile.py");
		Restore();

		std::cout << "[INFO] Shaders compilation has ended." << std::endl;
	}

	void Compiler::Read()
	{
		int id = 0;
		for (const auto& path : { COMPILER::RAY_HIT_SHADER, COMPILER::RAY_MISS_SHADER, COMPILER::RAY_GEN_SHADER })
		{
			std::ifstream inShader(path);
			std::string line;
			int i = 0;
			std::vector<std::string> file;
			while (std::getline(inShader, line))
			{
				file.push_back(line);

				if (line.find(COMPILER::TOKEN_INTEGRATOR) != std::string::npos)
					tokens[COMPILER::TOKEN_INTEGRATOR] = i;

				if (line.find(COMPILER::TOKEN_DEFINES) != std::string::npos)
					tokens[COMPILER::TOKEN_DEFINES] = i;

				++i;
			}
			inShader.close();

			COMPILER::Shader shader;
			shader.path = path;
			shader.content = file;

			if (id == 0)
			{
				shader.type = COMPILER::RAY_HIT;
				shaders[COMPILER::RAY_HIT] = shader;
			}

			if (id == 1)
			{
				shader.type = COMPILER::RAY_MISS;
				shaders[COMPILER::RAY_MISS] = shader;
			}

			if (id == 2)
			{
				shader.type = COMPILER::RAY_GEN;
				shaders[COMPILER::RAY_GEN] = shader;
			}

			++id;
		}
	}

	void Compiler::Include()
	{
		std::ofstream outShader(COMPILER::RAY_HIT_SHADER);
		int i = 0;
		for (auto& line : shaders[COMPILER::RAY_HIT].content)
		{
			outShader << (i == tokens[COMPILER::TOKEN_INTEGRATOR] ? COMPILER::INTEGRATORS[integrator] : line) <<
				std::endl;
			++i;
		}
		outShader.close();
	}

	void Compiler::Define()
	{
		std::ofstream outShader(COMPILER::RAY_MISS_SHADER);
		int i = 0;
		for (auto& line : shaders[COMPILER::RAY_MISS].content)
		{
			if (i == tokens[COMPILER::TOKEN_DEFINES])
			{
				outShader << "#define USE_HDR" << std::endl;
			}
			else
			{
				outShader << line << std::endl;
			}
			++i;
		}
		outShader.close();
	}

	void Compiler::Restore()
	{
		for (const auto& pair : shaders)
		{
			auto shader = pair.second;
			std::ofstream outShader(shader.path);
			int i = 0;
			for (auto& line : shader.content)
			{
				outShader << line << std::endl;
			}
			outShader.close();
		}
	}
}

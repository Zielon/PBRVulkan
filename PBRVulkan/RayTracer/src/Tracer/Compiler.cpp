#include "Compiler.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace Tracer
{
	namespace Parser
	{
		struct Shader
		{
			std::string path;
			std::string extension;
			std::vector<std::string> content{};
			std::vector<int> definesTokens{};
			std::vector<int> includeTokens{};
		};

		std::string TOKEN_INTEGRATOR = "// ====== INTEGRATOR ======";
		std::string TOKEN_DEFINES = "// ====== DEFINES ======";

		std::string RAY_HIT_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing";
		std::string RAY_MISS_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing";
		std::string RAY_SHADOW_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Shadow";
		std::string RAY_GEN_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing";

		std::map<Include, std::string> INCLUDES = {
			{ Include::PATH_TRACER_DEFAULT, "#include \"Integrators/PathTracer.glsl\"" },
			{ Include::PATH_TRACER_MSM, "#include \"Integrators/PathTracerMSM.glsl\"" },
			{ Include::AMBIENT_OCCLUSION, "#include \"Integrators/AO.glsl\"" }
		};

		std::map<Define, std::string> DEFINES = {
			{ Define::USE_HDR, "#define USE_HDR" },
			{ Define::USE_GAMMA_CORRECTION, "#define USE_GAMMA_CORRECTION" }
		};

		std::map<ShaderType, Shader> SHADERS = {
			{ ShaderType::RAY_HIT, { RAY_HIT_SHADER, ".rchit" } },
			{ ShaderType::RAY_MISS, { RAY_MISS_SHADER, ".rmiss" } },
			{ ShaderType::RAY_GEN, { RAY_GEN_SHADER, ".rgen" } },
			{ ShaderType::RAY_SHADOW, { RAY_SHADOW_SHADER, ".rmiss" } }
		};
	}

	Compiler::Compiler()
	{
		Read();
	}

	void Compiler::Read() const
	{
		for (const auto& pair : Parser::SHADERS)
		{
			auto shader = pair.second;
			std::ifstream inShader(shader.path + shader.extension);
			std::string line;
			int i = 0;
			std::vector<std::string> file;
			while (std::getline(inShader, line))
			{
				file.push_back(line);

				if (line.find(Parser::TOKEN_INTEGRATOR) != std::string::npos)
					shader.includeTokens.push_back(i);

				if (line.find(Parser::TOKEN_DEFINES) != std::string::npos)
					shader.definesTokens.push_back(i);

				++i;
			}
			inShader.close();
			shader.content = file;
			Parser::SHADERS[pair.first] = shader;
		}
	}

	/*
	 * Includes integrator selected form the menu
	 */
	void Compiler::Compile(std::vector<Parser::Include> includes, std::vector<Parser::Define> defines) const
	{
		std::cout << "[COMPILER] Shaders compilation has begun." << std::endl;

		for (const auto& pair : Parser::SHADERS)
		{
			const auto& shader = pair.second;
			std::ofstream outShader(shader.path + ".compiled" + shader.extension, std::ofstream::trunc);
			int i = 0;
			for (const auto& line : shader.content)
			{
				bool isOriginal = true;

				if (std::find(
					shader.includeTokens.begin(), shader.includeTokens.end(), i) != shader.includeTokens.end())
				{
					isOriginal = false;
					for (auto include : includes)
						outShader << Parser::INCLUDES[include] << std::endl;
				}

				if (std::find(
					shader.definesTokens.begin(), shader.definesTokens.end(), i) != shader.definesTokens.end())
				{
					isOriginal = false;
					for (auto define : defines)
						outShader << Parser::DEFINES[define] << std::endl;
				}

				if (isOriginal)
				{
					outShader << line << std::endl;
				}

				++i;
			}
			outShader.close();
		}

		std::system("python ./scripts/Compile.py");

		std::cout << "[COMPILER] Shaders compilation has ended." << std::endl;
	}
}

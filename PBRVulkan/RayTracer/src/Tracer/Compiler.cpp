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
			std::vector<std::string> content{};
			std::vector<int> definesTokens{};
			std::vector<int> includeTokens{};
		};

		std::string TOKEN_INTEGRATOR = "// ====== INTEGRATOR ======";
		std::string TOKEN_DEFINES = "// ====== DEFINES ======";

		std::string RAY_HIT_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rchit";
		std::string RAY_MISS_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rmiss";
		std::string RAY_GEN_SHADER = "../RayTracer/src/Assets/Shaders/Raytracer/Raytracing.rgen";

		std::map<Include, std::string> INCLUDES = {
			{ PATH_TRACER_DEFAULT, "#include \"Integrators/PathTracer.glsl\"" },
			{ PATH_TRACER_MSM, "#include \"Integrators/PathTracerMSM.glsl\"" }
		};

		std::map<Define, std::string> DEFINES = {
			{ USE_HDR, "#define USE_HDR" },
		};

		std::map<ShaderType, Shader> SHADERS = {
			{ RAY_HIT, { RAY_HIT_SHADER } },
			{ RAY_MISS, { RAY_MISS_SHADER } },
			{ RAY_GEN, { RAY_GEN_SHADER } },
		};
	}

	Compiler::Compiler(std::vector<Parser::Include> includes, std::vector<Parser::Define> defines)
		: defines(std::move(defines)), includes(std::move(includes))
	{
		std::cout << "[INFO] Shaders compilation has begin." << std::endl;

		Read();
		Compile();
		std::system("python ./scripts/Compile.py");
		Restore();

		std::cout << "[INFO] Shaders compilation has ended." << std::endl;
	}

	void Compiler::Read() const
	{
		for (const auto& pair : Parser::SHADERS)
		{
			auto shader = pair.second;
			std::ifstream inShader(shader.path);
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
	void Compiler::Compile() const
	{
		for (const auto& pair : Parser::SHADERS)
		{
			const auto& shader = pair.second;
			std::ofstream outShader(shader.path);
			int i = 0;
			for (const auto& line : shader.content)
			{
				if (std::find(
					shader.includeTokens.begin(), shader.includeTokens.end(), i) != shader.includeTokens.end())
				{
					for (auto include : includes)
						outShader << Parser::INCLUDES[include] << std::endl;
				}
				else if (std::find(
					shader.definesTokens.begin(), shader.definesTokens.end(), i) != shader.definesTokens.end())
				{
					for (auto define : defines)
						outShader << Parser::DEFINES[define] << std::endl;
				}
				else
				{
					outShader << line << std::endl;
				}
				++i;
			}
			outShader.close();
		}
	}

	void Compiler::Restore() const
	{
		for (const auto& pair : Parser::SHADERS)
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

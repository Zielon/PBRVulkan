#include "Compiler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "../path.h"

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

		std::string RAY_HIT_SHADER = "src/Assets/Shaders/Raytracer/Raytracing";
		std::string RAY_MISS_SHADER = "src/Assets/Shaders/Raytracer/Raytracing";
		std::string RAY_SHADOW_SHADER = "src/Assets/Shaders/Raytracer/Shadow";
		std::string RAY_GEN_SHADER = "src/Assets/Shaders/Raytracer/Raytracing";

		std::map<Include, std::string> INCLUDES = {
			{Include::PATH_TRACER_DEFAULT, "#include \"Integrators/PathTracer.glsl\""},
			{Include::PATH_TRACER_MSM, "#include \"Integrators/PathTracerMSM.glsl\""},
			{Include::AMBIENT_OCCLUSION, "#include \"Integrators/AO.glsl\""}
		};

		std::map<Define, std::string> DEFINES = {
			{Define::USE_HDR, "#define USE_HDR"},
			{Define::USE_GAMMA_CORRECTION, "#define USE_GAMMA_CORRECTION"}
		};

		std::map<ShaderType, Shader> SHADERS = {
			{ShaderType::RAY_HIT, {RAY_HIT_SHADER, ".rchit"}},
			{ShaderType::RAY_MISS, {RAY_MISS_SHADER, ".rmiss"}},
			{ShaderType::RAY_GEN, {RAY_GEN_SHADER, ".rgen"}},
			{ShaderType::RAY_SHADOW, {RAY_SHADOW_SHADER, ".rmiss"}}
		};
	}

	Compiler::Compiler()
	{
		root = Path::Root({"PBRVulkan", "RayTracer"});
		Read();
	}

	void Compiler::Read() const
	{
		for (const auto& pair : Parser::SHADERS)
		{
			auto shader = pair.second;
			auto name = std::filesystem::path(shader.path + shader.extension).make_preferred();
			auto path = (root / name).string();
			std::ifstream inShader(path);
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

	void Compiler::GlslangValidator() const
	{
		auto output_folder = root / ".." / "Assets" / "Shaders";
		auto shaders = root / "src" / "Assets" / "Shaders";
		std::vector<std::string> extensionsToCheck = {
			".frag", ".vert", ".comp", ".compiled.rchit", ".compiled.rgen", ".compiled.rmiss"
		};

		std::filesystem::create_directory(output_folder);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(shaders))
		{
			if (entry.is_directory())
				continue;

			auto path = entry.path();

			for (const auto& extension : extensionsToCheck)
			{
				if (path.filename().string().find(extension) != std::string::npos)
				{
					auto shader = path;
					shader.replace_extension(path.extension().string() + ".spv");
					auto output = output_folder / shader.filename();
					auto cmd = "glslangValidator --target-env vulkan1.2 -V " + path.string() + " -o " + output.string();
					std::system(cmd.c_str());
				}
			}
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
			auto name = std::filesystem::path(shader.path + ".compiled" + shader.extension).make_preferred();
			auto path = (root / name).string();
			std::ofstream outShader(path, std::ofstream::trunc);
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

		GlslangValidator();

		std::cout << "[COMPILER] Shaders compilation has ended." << std::endl;
	}
}

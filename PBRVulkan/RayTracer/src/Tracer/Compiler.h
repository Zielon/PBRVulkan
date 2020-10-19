#pragma once

#include <map>
#include <vector>

#include "../Vulkan/Vulkan.h"

namespace Tracer
{
	enum Integrator
	{
		PATH_TRACER_DEFAULT,
		PATH_TRACER_MSM
	};

	namespace COMPILER
	{
		enum Type
		{
			RAY_GEN,
			RAY_MISS,
			RAY_HIT
		};

		struct Shader
		{
			Type type;
			std::string path;
			std::vector<std::string> content;
		};
	}


	class Compiler
	{
	public:
		NON_COPIABLE(Compiler)

		Compiler(Integrator integrator, bool define);
		~Compiler() = default;

	private:
		Integrator integrator{};
		std::map<COMPILER::Type, COMPILER::Shader> shaders;
		std::map<std::string, int> tokens;

		void Read();
		void Include();
		void Define();
		void Restore();
	};
}

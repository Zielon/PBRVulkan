#pragma once

#include <vector>

#include "../Vulkan/Vulkan.h"

namespace Tracer
{
	enum Integrator
	{
		PATH_TRACER_DEFAULT,
		PATH_TRACER_MSM
	};

	class Compiler
	{
	public:
		NON_COPIABLE(Compiler)

		Compiler(Integrator integrator);
		~Compiler() = default;

		void Compile();

	private:
		Integrator integrator{};
		std::vector<std::string> file;
		int tokenPosition = 0;

		void Read();
		void Include();
		void Restore();
	};
}

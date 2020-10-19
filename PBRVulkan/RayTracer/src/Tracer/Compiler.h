#pragma once

#include <map>
#include <vector>

#include "../Vulkan/Vulkan.h"

namespace Tracer
{
	namespace Parser
	{
		enum Defines
		{
			DEFINE_USE_HDR
		};

		enum Include
		{
			INCLUDE_PATH_TRACER_DEFAULT,
			INCLUDE_PATH_TRACER_MSM
		};

		enum ShaderType
		{
			RAY_GEN,
			RAY_MISS,
			RAY_HIT
		};
	}

	class Compiler
	{
	public:
		NON_COPIABLE(Compiler)

		Compiler(Parser::Include integrator, std::vector<Parser::Defines> defines);
		~Compiler() = default;

	private:
		Parser::Include integrator{};
		std::vector<Parser::Defines> defines;

		void Read() const;
		void Compile() const;
		void Restore() const;
	};
}

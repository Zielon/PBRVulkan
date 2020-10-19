#pragma once

#include <map>
#include <vector>

#include "../Vulkan/Vulkan.h"

namespace Tracer
{
	namespace Parser
	{
		enum Define
		{
			USE_HDR
		};

		enum Include
		{
			PATH_TRACER_DEFAULT,
			PATH_TRACER_MSM
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

		Compiler(std::vector<Parser::Include> includes, std::vector<Parser::Define> defines);
		~Compiler() = default;

	private:
		std::vector<Parser::Define> defines;
		std::vector<Parser::Include> includes;

		void Read() const;
		void Compile() const;
		void Restore() const;
	};
}

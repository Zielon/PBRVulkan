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

		Compiler();
		~Compiler() = default;
		void Compile(std::vector<Parser::Include> include, std::vector<Parser::Define> defines) const;
		
	private:
		void Read() const;
		void Restore() const;
	};
}

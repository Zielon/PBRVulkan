#pragma once

#include <map>
#include <vector>
#include <algorithm>

#include "../Vulkan/Vulkan.h"

namespace Tracer
{
	namespace Parser
	{
		enum class Define
		{
			USE_HDR,
			USE_GAMMA_CORRECTION
		};

		enum class Include
		{
			PATH_TRACER_DEFAULT,
			PATH_TRACER_MSM,
			AMBIENT_OCCLUSION
		};

		enum class ShaderType
		{
			RAY_GEN,
			RAY_MISS,
			RAY_SHADOW,
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
	};
}

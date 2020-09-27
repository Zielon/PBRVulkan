#pragma once

#include <string>

namespace Tracer
{
	class Scene;
}

namespace Loader
{
	bool LoadSceneFromFile(
		const std::string& filename,
		class Tracer::Scene& scene,
		struct RenderOptions& renderOptions);

	extern int (*Log)(const char* szFormat, ...);
}

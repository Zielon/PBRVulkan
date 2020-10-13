#pragma once

#include "Vulkan.h"

#include <memory>

namespace Vulkan
{
	class ShaderBindingTable
	{
	public:
		NON_COPIABLE(ShaderBindingTable)

		ShaderBindingTable(const class RaytracerGraphicsPipeline& raytracerPipeline);
		~ShaderBindingTable();

	private:
		const RaytracerGraphicsPipeline& raytracerPipeline;

		std::unique_ptr<class Extensions> extensions;
		std::unique_ptr<class Buffer> stbBuffer;
	};
}

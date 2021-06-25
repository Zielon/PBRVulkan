#pragma once

#include "Vulkan.h"
#include <cstring>
#include <memory>

namespace Vulkan
{
	/*
	 * The SBT is an array containing the handles to the shader groups used in the ray tracing pipeline.
	 */
	class ShaderBindingTable
	{
	public:
		NON_COPIABLE(ShaderBindingTable)

		ShaderBindingTable(const class RaytracerGraphicsPipeline& raytracerPipeline);
		~ShaderBindingTable();

		[[nodiscard]] size_t GetEntrySize() const
		{
			return entrySize;
		}
		
		[[nodiscard]] const class Buffer& GetBuffer() const
		{
			return *stbBuffer;
		}

	private:
		const RaytracerGraphicsPipeline& raytracerPipeline;

		size_t entrySize;
		std::unique_ptr<class Extensions> extensions;
		std::unique_ptr<class Buffer> stbBuffer;
	};
}

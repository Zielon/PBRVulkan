#include "ShaderBindingTable.h"

#include "Buffer.h"
#include "Extensions.h"
#include "RaytracerGraphicsPipeline.h"

namespace Vulkan
{
	ShaderBindingTable::ShaderBindingTable(const RaytracerGraphicsPipeline& raytracerPipeline)
		: raytracerPipeline(raytracerPipeline)
	{
		extensions.reset(new Extensions(raytracerPipeline.GetDevice()));

		//stbBuffer.reset(new Buffer(raytracerPipeline.GetDevice(), ));
	}

	ShaderBindingTable::~ShaderBindingTable()
	{
		extensions.reset();
		stbBuffer.reset();
	}
}

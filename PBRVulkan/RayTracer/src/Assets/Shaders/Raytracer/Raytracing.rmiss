#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

// Replaced by Compiler.h

#include "../Common/Structs.glsl"

layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };

#ifdef USE_HDR
layout(binding = 10) uniform sampler2D[] HDRs;
#endif

layout(location = 0) rayPayloadInNV RayPayload payload;

void main()
{
	payload.throughput = vec3(1, 0, 0);

	if (ubo.useHDR)
	{
		#ifdef USE_HDR
		payload.radiance += texture(HDRs[0], uv).xyz;
		#endif
	}
	else
	{
		payload.radiance = vec3(0);
	}
}

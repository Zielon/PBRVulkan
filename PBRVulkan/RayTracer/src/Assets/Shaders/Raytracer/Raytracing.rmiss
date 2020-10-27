#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

// Replaced by Compiler.h
// ====== DEFINES ======

#include "../Common/Structs.glsl"

layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };

#ifdef USE_HDR
layout(binding = 10) uniform sampler2D[] HDRs;
#endif

layout(location = 0) rayPayloadInNV RayPayload payload;

void main()
{
	vec3 direction = gl_WorldRayDirectionNV;

	if (ubo.useHDR)
	{
		#ifdef USE_HDR
		vec2 uv = vec2((PI + atan(direction.z, direction.x)) * (1.0 / TWO_PI), acos(direction.y) * (1.0 / PI));
		payload.radiance = texture(HDRs[0], uv).xyz;
		#endif
	}
	else
	{
		payload.radiance = vec3(0);
	}
}

#version 460

precision highp float;
precision highp int;

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

// Replaced by Compiler.h
// ====== DEFINES ======

#include "../Common/Structs.glsl"

layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };

#include "../Common/Composition.glsl"

#ifdef USE_HDR
layout(binding = 10) uniform sampler2D[] HDRs;
#include "../Common/HDR.glsl"
#endif

layout(location = 0) rayPayloadInNV RayPayload payload;

void main()
{
	if (ubo.useHDR)
	{
		#ifdef USE_HDR
		float lightPdf = 1.0f;
		float misWeight = 1.0f;
		float hdrMultiplier = 5.0f;
		vec2 uv = vec2((PI + atan(gl_WorldRayDirectionNV.z, gl_WorldRayDirectionNV.x)) * (1.0 / TWO_PI), acos(gl_WorldRayDirectionNV.y) * (1.0 / PI));
		if (payload.depth > 0 && !payload.specularBounce)
		{
			lightPdf = envPdf();
			misWeight = powerHeuristic(0, lightPdf);
		}
		payload.radiance += misWeight * texture(HDRs[0], uv).xyz * payload.throughput * hdrMultiplier;
		#endif
	}
}

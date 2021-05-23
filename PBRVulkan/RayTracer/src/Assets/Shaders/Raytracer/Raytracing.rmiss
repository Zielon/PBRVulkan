#version 460

precision highp float;
precision highp int;

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require

// Replaced by Compiler.h
// ====== DEFINES ======

#include "../Common/Structs.glsl"

layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 9) readonly buffer LightArray { Light[] Lights; };

layout(location = 0) rayPayloadInEXT RayPayload payload;

#include "../Common/Random.glsl"
#include "../Common/Math.glsl"
#include "../Common/Sampling.glsl"

#ifdef USE_HDR
layout(binding = 12) uniform sampler2D[] HDRs;
#include "../Common/HDR.glsl"
#endif

void main()
{
	// Stop path tracing loop from rgen shader
	payload.stop = true;
	payload.ffnormal = vec3(0.);
	payload.worldPos = vec3(0.);
	
	if (ubo.integratorType == AMBIENT_OCCLUSION)
	{
		return;
	}

	LightSample lightSample;

	if (interesetsEmitter(lightSample, INFINITY))
	{
		vec3 Le = sampleEmitter(lightSample, payload.bsdf);
		payload.radiance += Le * payload.beta;
		return;
	}

	if (ubo.useHDR)
	{
		#ifdef USE_HDR
		float lightPdf = 1.0f;
		float misWeight = 1.0f;
		float hdrMultiplier = 7.0f;
		vec2 uv = vec2((PI + atan(gl_WorldRayDirectionEXT.z, gl_WorldRayDirectionEXT.x)) * INV_2PI, acos(gl_WorldRayDirectionEXT.y) * INV_PI);
		if (payload.depth > 0 && !payload.specularBounce)
		{
			lightPdf = envPdf();
			misWeight = powerHeuristic(0, lightPdf);
		}
		payload.radiance += misWeight * texture(HDRs[0], uv).xyz * payload.beta * hdrMultiplier;
		#endif
	}
}

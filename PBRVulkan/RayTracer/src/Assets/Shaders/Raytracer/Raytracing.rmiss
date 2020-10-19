#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "../Common/Structs.glsl"

layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 10) uniform sampler2D[] HDRs;

layout(location = 0) rayPayloadInNV RayPayload Ray;

void main()
{
	if (ubo.useHDR)
	{
		vec2 uv = vec2((PI + atan(Ray.direction.z, Ray.direction.x)) * (1.0 / TWO_PI), acos(Ray.direction.y) * (1.0 / PI));
		Ray.color = texture(HDRs[0], uv).rgb;
	}
	else
	{
		Ray.color = vec3(0);
	}
}

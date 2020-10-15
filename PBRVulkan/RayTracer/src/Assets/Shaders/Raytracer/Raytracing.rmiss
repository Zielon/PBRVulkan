#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "../Common/Structs.glsl"

layout(location = 0) rayPayloadInNV RayPayload Ray;

void main()
{
	Ray.color = vec3(0, 0, 0);
}

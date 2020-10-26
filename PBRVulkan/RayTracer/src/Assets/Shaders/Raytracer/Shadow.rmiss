#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "../Common/Structs.glsl"

layout(location = 0) rayPayloadInNV RayPayload payload;
layout(location = 1) rayPayloadInNV bool isShadowed;

void main()
{
	isShadowed = false;
}

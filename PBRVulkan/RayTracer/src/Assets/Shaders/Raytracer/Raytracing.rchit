#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "Integrators/Default.glsl"
#include "../Common/Structs.glsl"

layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;

hitAttributeNV vec2 Hit;
rayPayloadInNV RayPayload Ray;

Vertex unpack(uint index)
{
	const uint vertexSize = 9;
	const uint offset = index * vertexSize;
	
	Vertex v;
	
	v.position = vec3(Vertices[offset + 0], Vertices[offset + 1], Vertices[offset + 2]);
	v.normal = vec3(Vertices[offset + 3], Vertices[offset + 4], Vertices[offset + 5]);
	v.texCoord = vec2(Vertices[offset + 6], Vertices[offset + 7]);
	v.materialIndex = floatBitsToInt(Vertices[offset + 8]);

	return v;
}

vec3 mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics) 
{
    return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

void main()
{
	uvec2 offsets = Offsets[gl_InstanceCustomIndexNV];
	uint indexOffset = offsets.x;
	uint vertexOffset = offsets.y;

	const Vertex v0 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
	const Vertex v1 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
	const Vertex v2 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);

	const Material material = Materials[v0.materialIndex];

	const vec3 barycentrics = vec3(1.0 - Hit.x - Hit.y, Hit.x, Hit.y);
	const vec3 normal = normalize(mix(v0.normal, v1.normal, v2.normal, barycentrics));

//	Ray.colorAndDistance = vec4(material.albedo.rgb, 1);
	Ray.colorAndDistance = vec4(abs(normal.rgb), 1);

}


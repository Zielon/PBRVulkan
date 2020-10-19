#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "../Common/Structs.glsl"

layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;

#include "../Common/Vertex.glsl"
#include "../Common/Math.glsl"

hitAttributeNV vec2 Hit;
rayPayloadInNV RayPayload Ray;

void main()
{
	uvec2 offsets = Offsets[gl_InstanceCustomIndexNV];
	uint indexOffset = offsets.x;
	uint vertexOffset = offsets.y;

	const Vertex v0 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
	const Vertex v1 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
	const Vertex v2 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);

	const Material material = Materials[v0.materialIndex];

	// Replaced by Compiler.h
	// ====== INTEGRATOR ======
}

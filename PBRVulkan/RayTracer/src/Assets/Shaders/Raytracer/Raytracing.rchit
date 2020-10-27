#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

// Replaced by Compiler.h

#include "../Common/Structs.glsl"

layout(binding = 0, set = 0) uniform accelerationStructureNV TLAS;
layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;
layout(binding = 9) readonly buffer LightArray { Light[] Lights; };

#ifdef USE_HDR
layout(binding = 10) uniform sampler2D[] HDRs;
#endif

layout(location = 0) rayPayloadInNV RayPayload payload;
layout(location = 1) rayPayloadNV bool isShadowed;

hitAttributeNV vec2 Hit;

#include "../Common/Vertex.glsl"
#include "../Common/Math.glsl"
#include "../Common/Random.glsl"

vec3 sampleLight(Light light)
{
	float r1 = rnd(payload.seed);
	float r2 = rnd(payload.seed);
	return light.position + light.u * r1 + light.v * r2;
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
	const vec2 texCoord = mix(v0.texCoord, v1.texCoord, v2.texCoord, barycentrics);
	const vec3 worldPos = mix(v0.position, v1.position, v2.position, barycentrics);

	if (material.albedoTexID >= 0)
		payload.radiance = texture(TextureSamplers[material.albedoTexID], texCoord).rbg;
	else
		payload.radiance = material.albedo.rbg;

	if (ubo.lights == 0)
		return;

	Light light;
	int index = int(rnd(payload.seed) * float(ubo.lights));
	light = Lights[index];

	vec3 lightDir = sampleLight(light) - worldPos;
	float lightDist = length(lightDir);
	float lightDistSq = lightDist * lightDist;
	lightDir /= sqrt(lightDistSq);
	
	if (dot(normal, lightDir) > 0)
	{
		float tMin     = 0.001;
		float tMax     = lightDist;
		uint flags     = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;
		vec3 origin    = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV - EPS;
		isShadowed     = true;

		traceNV(TLAS,           // acceleration structure
				flags,          // rayFlags
				0xFF,           // cullMask
				0,              // sbtRecordOffset
				0,              // sbtRecordStride
				1,              // missIndex
				origin,         // ray origin
				tMin,           // ray min range
				lightDir,       // ray direction
				tMax,           // ray max range
				1               // payload (location = 1)
		);

		if (isShadowed)
		{
			payload.radiance *= 0.3;
		}
	}
}

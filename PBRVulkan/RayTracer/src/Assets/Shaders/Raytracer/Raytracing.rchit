#version 460

precision highp float;
precision highp int;

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require

// Replaced by Compiler.h
// ====== DEFINES ======

#include "../Common/Structs.glsl"

layout(binding = 0, set = 0) uniform accelerationStructureEXT TLAS;
layout(binding = 3) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;
layout(binding = 9) readonly buffer LightArray { Light[] Lights; };

#ifdef USE_HDR
layout(binding = 12) uniform sampler2D[] HDRs;
#endif

layout(location = 0) rayPayloadInEXT RayPayload payload;
layout(location = 1) rayPayloadEXT bool isShadowed;

hitAttributeEXT vec2 hit;

#include "../Common/Vertex.glsl"
#include "../Common/Random.glsl"
#include "../Common/Math.glsl"

#ifdef USE_HDR
#include "../Common/HDR.glsl"
#endif

#include "../Common/Sampling.glsl"
#include "../BSDFs/UE4BSDF.glsl"
#include "../BSDFs/DisneyBSDF.glsl"

#include "../Common/DirectLight.glsl"

void main()
{
	uvec2 offsets = Offsets[gl_InstanceCustomIndexEXT];
	uint indexOffset = offsets.x;
	uint vertexOffset = offsets.y;

	const Vertex v0 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
	const Vertex v1 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
	const Vertex v2 = unpack(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);

	Material material = Materials[v0.materialIndex];

	const vec3 barycentrics = vec3(1.0 - hit.x - hit.y, hit.x, hit.y);
	const vec2 texCoord = mix(v0.texCoord, v1.texCoord, v2.texCoord, barycentrics);
	const vec3 worldPos = mix(v0.position, v1.position, v2.position, barycentrics);
	vec3 normal = normalize(mix(v0.normal, v1.normal, v2.normal, barycentrics));
	// face forward normal
	vec3 ffnormal = dot(normal, gl_WorldRayDirectionEXT) <= 0.0 ? normal : normal * -1.0;
	float eta = dot(normal, ffnormal) > 0.0 ? (1.0 / material.ior) : material.ior;

	// Update the material properties using textures
	
	// Albedo
	if (material.albedoTexID >= 0)
	{
		material.albedo.xyz *= texture(TextureSamplers[material.albedoTexID], texCoord).xyz;
	}

	// Metallic and Roughness
	if (material.metallicRoughnessTexID >= 0)
	{
		vec2 metallicRoughness = texture(TextureSamplers[material.metallicRoughnessTexID], texCoord).zy;
		material.metallic = metallicRoughness.x;
		material.roughness = metallicRoughness.y;
	}

	// Normal map
	if (material.normalmapTexID >= 0)
	{
		// Orthonormal Basis
		mat3 frame = localFrame(ffnormal);
		vec3 nrm = texture(TextureSamplers[material.normalmapTexID], texCoord).xyz;
		nrm = frame * normalize(nrm * 2.0 - 1.0);
		normal = normalize(nrm);
		ffnormal = dot(normal, gl_WorldRayDirectionEXT) <= 0.0 ? normal : normal * -1.0;
	}
	
	payload.worldPos = worldPos;
	payload.normal = normal;
	payload.ffnormal = ffnormal;
	payload.eta = eta;

	seed = tea(gl_LaunchIDEXT.y * gl_LaunchSizeEXT.x + gl_LaunchIDEXT.x, ubo.frame);

	// vec3 hit = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

	// Replaced by Compiler.h
	// ====== INTEGRATOR ======
}

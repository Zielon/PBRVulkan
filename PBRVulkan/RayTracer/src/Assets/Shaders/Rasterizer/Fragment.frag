#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "../Common/Structs.glsl"

layout(binding = 1) buffer MaterialArray { Material[] materials; };
layout(binding = 2) uniform sampler2D[] textureSamplers;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inDirection;
layout(location = 3) in flat int inMaterialId;

layout(location = 0) out vec4 outColor;

vec3 tone_map(in vec3 color, float limit)
{
	float luminance = 0.3 * color.x + 0.6 * color.y + 0.1 * color.z;
	return color * 1.0 / (1.0 + luminance / limit);
}

void main() 
{
	vec3 normal = normalize(inNormal);
	Material material = materials[inMaterialId];

	int textureId = material.albedoTexID;
	float d = max(dot(inDirection, normalize(normal)), 0.2);

	vec3 color = vec3(0);

	if (textureId >= 0)
		color = texture(textureSamplers[textureId], inTexCoord).rgb;
	else
		color = material.albedo.xyz;

	outColor = vec4(pow(tone_map(color, 1.5), vec3(1.0 / 2.2)), 1.0);
}
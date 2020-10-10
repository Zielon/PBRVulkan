#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "Structs.glsl"

layout(binding = 2) uniform sampler2D[] textureSamplers;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inDirection;
layout(location = 3) in Material inMaterial;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec3 normal = normalize(inNormal);
	int textureId = int(inMaterial.texIDs.x);
	float d = max(dot(inDirection, normalize(normal)), 0.2);

	if (textureId >= 0)
	{
		vec3 color = texture(textureSamplers[textureId], inTexCoord).rgb;
		outColor = vec4(color, 1.0);
	}
	else
	{
		discard;
	}
}
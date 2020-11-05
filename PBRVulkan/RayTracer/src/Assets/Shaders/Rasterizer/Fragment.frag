#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "../Common/Structs.glsl"

layout(binding = 0) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 1) readonly buffer MaterialArray { Material[] materials; };
layout(binding = 2) uniform sampler2D[] textureSamplers;
layout(binding = 3) readonly buffer LightArray { Light[] Lights; };

#include "../Common/Math.glsl"

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inDirection;
layout(location = 3) in flat int inMaterialId;

layout(location = 0) out vec4 outColor;

void main()
{
	Material material = materials[inMaterialId];

	vec3 normal = normalize(inNormal);
	int textureId = material.albedoTexID;
	vec3 color = vec3(0);

	for (uint i = 0; i < ubo.lights; ++i)
	{
		Light light = Lights[i];
		// TODO
	}

	if (textureId >= 0)
		color = texture(textureSamplers[textureId], inTexCoord).rgb;
	else
		color = material.albedo.xyz;

	outColor = vec4(gammaCorrection(toneMap(color, 1.5)), 1.0);
}
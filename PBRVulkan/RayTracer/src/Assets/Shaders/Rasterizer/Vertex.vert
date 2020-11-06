#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

#include "../Common/Structs.glsl"

layout(binding = 0) readonly uniform UniformBufferObject { Uniform ubo; };

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inMaterialId;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outDirection;
layout(location = 3) out flat int outMaterialId;
layout(location = 4) out vec3 outPosition;

void main() 
{
	mat4 normalMat = transpose(inverse(ubo.view));//Note: precompute outside shader

	gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);

	//outNormal = inNormal;
	outNormal = vec3(normalMat * vec4(inNormal, 0.0));
	outTexCoord = inTexCoord;
	outDirection = ubo.direction;
	vec4 pos = ubo.view * vec4(inPosition, 1.0);
	outPosition = pos.xyz;
	outMaterialId = inMaterialId;
}
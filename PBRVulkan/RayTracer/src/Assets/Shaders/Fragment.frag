#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inDirection;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec3 normal = normalize(inNormal);
    outColor = vec4(abs(normal), 1.0);
}
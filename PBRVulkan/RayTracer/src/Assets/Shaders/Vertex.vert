#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

#include "Structs.glsl"

layout(binding = 0) uniform UniformBufferObject { Uniform ubo; };
layout(binding = 1) buffer MaterialArray { Material[] materials; };

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inMaterialId;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outDirection;
layout(location = 3) out Material outMaterial;

void main() 
{
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);

    outNormal = inNormal;
    outTexCoord = inTexCoord;
    outDirection = ubo.direction;
    outMaterial = materials[inMaterialId];
}
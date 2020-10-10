#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 direction;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 outFagTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outDirection;

void main() {
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);

    outNormal = inNormal;
    outFagTexCoord = inTexCoord;
    outDirection = ubo.direction;
}
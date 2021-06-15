#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "../Common/Structs.glsl"

layout(binding = 0) readonly uniform UniformBufferObject { Uniform ubo; };
layout(binding = 1) readonly buffer MaterialArray { Material[] materials; };
layout(binding = 2) uniform sampler2D[] textureSamplers;
layout(binding = 3) readonly buffer LightArray { Light[] Lights; };

#include "../Common/Random.glsl"
#include "../Common/Math.glsl"

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;
layout(location = 3) in flat int inMaterialId;

layout(location = 0) out vec4 outColor;

void main()
{
	Material material = materials[inMaterialId];
	int textureId = material.albedoTexID;
	vec3 ka = vec3(0.1);
	vec3 kd = vec3(0.8);
	vec3 ks = vec3(0.9);
	float shininess = 32.0f;

	vec3 normal = normalize(inNormal);
	vec3 viewDir = normalize(ubo.cameraPos.xyz - inPosition);

	vec3 color = vec3(0);
	vec3 albedo = vec3(0);

	if (textureId >= 0)
		albedo = texture(textureSamplers[textureId], inTexCoord).rgb;
	else
		albedo = material.albedo.xyz;

	for (uint i = 0; i < ubo.lights; ++i)
	{
		Light light = Lights[i];
		
		vec3 lightPos;

		if (int(light.type) == AREA_LIGHT)
			// The center of an area light source
			lightPos = light.position + 0.5 * light.v + 0.5 * light.u;
		else 
			lightPos = light.position;

		vec3 lightDir = normalize(lightPos - inPosition);

		float cosTheta = max(dot(normal, lightDir), 0.0);
		float distance = length(lightPos - inPosition);
		float attenuation = 1.0 / (distance * distance);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

		vec3 ambient = ka * albedo;
		vec3 diffuse = kd * cosTheta * albedo;
		vec3 specular = ks * spec * albedo;

		color += (ambient + diffuse + specular) * attenuation;
	}

	outColor = vec4(gammaCorrection(toneMap(color, 1.5)), 1.0);
}
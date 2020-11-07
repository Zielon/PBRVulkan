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
layout(location = 2) in vec3 inPosition;
layout(location = 3) in flat int inMaterialId;

layout(location = 0) out vec4 outColor;

void main()
{
	Material material = materials[inMaterialId];
	vec3 albedo = material.albedo.xyz;
	float shininess = 80.0f;
	int textureId = material.albedoTexID;
	
	vec3 normal = normalize(inNormal);
	vec3 viewDir = normalize(-inPosition);//Camera position in eye coordinates is (0,0,0)
	vec3 color = vec3(0);

	// ambient term
	color += vec3(0.1);

	for (uint i = 0; i < ubo.lights; ++i)
	{
		Light light = Lights[i];

		// diffuse term
		vec3 lightPos = vec3 (ubo.view * vec4(light.position, 1.0));//Better to precomputed the light positions in eye space
		vec3 lightDir = normalize(lightPos - inPosition);
		float diffI = max(dot(lightDir, normal), 0.0);

		// specular term
		float specI = 0;

		if (diffI > 0.0){

			// Phong 
			vec3 halfwayDir = normalize(lightDir + viewDir);  
			specI = pow(max(dot(normal, halfwayDir), 0.0), shininess);

			// Blinn-Phong
			//vec3 R = normalize(reflect(-lightDir, normal)); 
			//specI = pow( max(dot(R, ViewDir), 0.0), shininess);
		}

		color +=  albedo * diffI + vec3(0.9) * specI;
	}

	color = clamp (color, 0.0, 1.0);

	if (textureId >= 0)
		color = texture(textureSamplers[textureId], inTexCoord).rgb;

	outColor = vec4(gammaCorrection(toneMap(color, 1.5)), 1.0);
}
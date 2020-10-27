/*
 * Path tracing integral. 
 * Code excerpt executed in the Hit shader with the following available propeties:
 *
 * Available Uniforms:
 * [accelerationStructureNV TLAS, Uniform ubo, TextureSamplers[], Lights[]]
 *
 * Availavle variables:
 * [Material material, vec3 barycentrics, vec3 normal, vec2 texCoord, vec3 worldPos]
 * 
 * Available payloads:
 * [rayPayloadInNV RayPayload payload, rayPayloadNV bool isShadowed]
 */
{
	if (material.albedoTexID >= 0)
		payload.radiance = texture(TextureSamplers[material.albedoTexID], texCoord).rbg;
	else
		payload.radiance = material.albedo.rbg;

	if (ubo.lights == 0)
		return;

	Light light;
	int index = int(rnd(payload.seed) * float(ubo.lights));
	light = Lights[index];

	vec3 lightNormal = normalize(cross(light.u, light.v));
	vec3 lightDir = sampleLight(light) - worldPos;
	float lightDist = length(lightDir);
	float lightDistSq = lightDist * lightDist;
	lightDir /= sqrt(lightDistSq);

	isShadowed = true;

	// The light is visible from the surface. Less than 90° between vectors.
	if (dot(normal, lightDir) > 0.f)
	{
		float tMin     = 0.001;
		float tMax     = lightDist;
		uint flags     = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;
		vec3 origin    = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV - EPS;

		traceNV(TLAS,           // acceleration structure
				flags,          // rayFlags
				0xFF,           // cullMask
				0,              // sbtRecordOffset
				0,              // sbtRecordStride
				1,              // missIndex
				origin,         // ray origin
				tMin,           // ray min range
				lightDir,       // ray direction
				tMax,           // ray max range
				1               // payload (location = 1)
		);
	}

	if (isShadowed)
	{
		payload.radiance *= 0.3;
	}
}
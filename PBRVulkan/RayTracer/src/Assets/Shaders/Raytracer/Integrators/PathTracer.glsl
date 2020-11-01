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
	int index = int(rnd(seed) * float(ubo.lights));
	light = Lights[index];

	vec3 sampled = sampleLight(light);
	vec3 lightDir = sampled - worldPos;
	float lightDist = length(lightDir);
	float lightDistSq = lightDist * lightDist;
	lightDir /= sqrt(lightDistSq);

	isShadowed = true;

	vec3 surfacePos = worldPos + ffnormal * EPS;

	// The light is visible from the surface. Less than 90° between vectors.
	if (dot(normal, lightDir) > 0.f)
	{
		float tMin     = 0.001;
		float tMax     = lightDist - EPS;
		uint flags     = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;

		traceNV(TLAS, flags, 0xFF, 0, 0, 1, surfacePos, tMin, lightDir, tMax, 1);
	}

	if (isShadowed)
	{
		payload.radiance *= 0.2;
	}
}
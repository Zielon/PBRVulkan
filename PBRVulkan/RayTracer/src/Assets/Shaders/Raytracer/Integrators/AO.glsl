/*
 * Ambient occlusion intergal. 
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
	float tMin      = 0.00001;
	float tMax      = ubo.AORayLength;
	uint flags      = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;

	uint samples = 5;
	float ao = 1.f;

	for(uint i = 0; i < samples; ++i)
	{
		mat3 frame = localFrame(ffnormal);
		vec3 dir = normalize(frame * cosineSampleHemisphere());

		isShadowed = true;

		traceNV(TLAS, flags, 0xFF, 0, 0, 1, worldPos, tMin, dir, tMax, 1);

		if (isShadowed)
		{
			ao -= (1.0 / samples);
		}
	}

	payload.radiance = vec3(ao);
 }
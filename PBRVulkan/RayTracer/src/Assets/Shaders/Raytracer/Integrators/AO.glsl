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
	float tMin     = 0.001;
	float tMax     = ubo.AORayLength;
	uint flags     = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;
	vec3 origin    = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV - EPS;

	uint samples = 5;
	float ao = 1.f;

	for(uint i = 0; i < samples; ++i)
	{
		float r1 = rnd(payload.seed);
		float r2 = rnd(payload.seed);
		mat3 frame = localFrame(normal);
		vec3 dir = frame * cosineSampleHemisphere(r1, r2);

		isShadowed = true;

		traceNV(TLAS,           // acceleration structure
				flags,          // rayFlags
				0xFF,           // cullMask
				0,              // sbtRecordOffset
				0,              // sbtRecordStride
				1,              // missIndex
				origin,         // ray origin
				tMin,           // ray min range
				dir,            // ray direction
				tMax,           // ray max range
				1               // payload (location = 1)
		);

		if (isShadowed)
		{
			ao -= (1.0 / samples);
		}
	}

	payload.radiance = vec3(ao);
 }
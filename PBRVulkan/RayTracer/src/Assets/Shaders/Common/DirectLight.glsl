/*
 * Direct light estimator
 */

vec3 directLight(in Material material)
{
	vec3 L      = vec3(0);
	float tMin  = MINIMUM;
	float tMax  = INFINITY;
	uint flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;

	BsdfSample bsdfSample;

	vec3 surfacePos = payload.worldPos;

	/* Environment Light */
	if (ubo.useHDR)
	{
		#ifdef USE_HDR
		vec3 color     = vec3(0);
		vec4 dirPdf    = envSample(color);
		vec3 lightDir  = dirPdf.xyz;
		float lightPdf = dirPdf.w;

		isShadowed = true;

		// Shadow ray (payload 1 is Shadow.miss)
		traceNV(TLAS, flags, 0xFF, 0, 0, 1, surfacePos, tMin, lightDir, tMax, 1);

		if (!isShadowed)
		{
			float bsdfPdf = UE4Pdf(material, lightDir);
			float cosTheta = abs(dot(lightDir, payload.ffnormal));
			vec3 F = UE4Eval(material, lightDir);
			float misWeight = powerHeuristic(lightPdf, bsdfPdf);

			if (misWeight > 0.0)
				L += (misWeight * F * cosTheta * color) / lightPdf;
		}
		#endif
	}

	if (ubo.lights > 0)
	{
		Light light;
		int index = int(rnd(seed) * float(ubo.lights));
		light = Lights[index];

		LightSample sampled = sampleLight(light);	
		vec3 lightDir       = sampled.position - surfacePos;
		float lightDist     = length(lightDir);
		float lightDistSq   = lightDist * lightDist;
		lightDir = normalize(lightDir);

		isShadowed = true;

		// The light has to be visible from the surface. Less than 90° between vectors.
		if (dot(payload.ffnormal, lightDir) <= 0.0 || dot(lightDir, sampled.normal) >= 0.0)
			return L;

		// Shadow ray (payload 1 is Shadow.miss)
		traceRayEXT(TLAS, flags, 0xFF, 0, 0, 1, surfacePos, tMin, lightDir, lightDist, 1);
		
		if (!isShadowed)
		{
			float bsdfPdf = UE4Pdf(material, lightDir);
			float lightPdf = lightDistSq / (light.area * abs(dot(sampled.normal, lightDir)));
			float cosTheta = abs(dot(payload.ffnormal, lightDir));
			vec3 F = UE4Eval(material, lightDir);

			L += (powerHeuristic(lightPdf, bsdfPdf) * F * cosTheta * sampled.emission) / lightPdf;
		}
	}

	return L;
}
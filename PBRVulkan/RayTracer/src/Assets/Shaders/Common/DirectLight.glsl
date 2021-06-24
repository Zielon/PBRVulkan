/*
 * Direct light estimator
 */

vec3 directLight(in Material material)
{
	vec3 L = vec3(0);
	float tMin = MINIMUM;
	float tMax = INFINITY;
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;

	BsdfSample bsdfSample;

	vec3 surfacePos = payload.worldPos;

	/* Environment Light */
	#ifdef USE_HDR
	{
		vec3 color = vec3(0);
		vec4 dirPdf = envSample(color);
		vec3 lightDir = dirPdf.xyz;
		float lightPdf = dirPdf.w;

		isShadowed = true;

		// Shadow ray (payload 1 is Shadow.miss)
		traceRayEXT(TLAS, flags, 0xFF, 0, 0, 1, surfacePos, tMin, lightDir, tMax, 1);

		if (!isShadowed)
		{
			vec3 F = DisneyEval(material, lightDir, bsdfSample.pdf);

			float cosTheta = abs(dot(lightDir, payload.ffnormal));
			float misWeight = powerHeuristic(lightPdf, bsdfSample.pdf);

			if (misWeight > 0.0)
				L += misWeight * F * cosTheta * color / (lightPdf + EPS);
		}
	}
	#endif

	if (ubo.lights > 0)
	{
		Light light;
		int index = int(rnd(seed) * float(ubo.lights));
		light = Lights[index];

		// In the case there is only environmnet light and not analitic light
		// light type is set to -1 and discarded.
		if (light.type == -1)
			return L;

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
			vec3 F = DisneyEval(material, lightDir, bsdfSample.pdf);

			float lightPdf = lightDistSq / (light.area * abs(dot(sampled.normal, lightDir)));
			float cosTheta = abs(dot(payload.ffnormal, lightDir));
			float misWeight = powerHeuristic(lightPdf, bsdfSample.pdf);

			L += misWeight * F * cosTheta * sampled.emission / (lightPdf + EPS);
		}
	}

	return L;
}
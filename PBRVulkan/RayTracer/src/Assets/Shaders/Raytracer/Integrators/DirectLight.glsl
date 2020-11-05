/*
 * Direct light estimator
 */

vec3 directLight(in Material material)
{
	vec3 L      = vec3(0);
	float tMin  = MINIMUM;
	float tMax  = INFINITY;
	uint flags  = gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV;

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

		vec3 sampled      = sampleLight(light);
		vec3 lightNormal  = normalize(cross(light.u, light.v));
		vec3 emission     = light.emission * float(ubo.lights);
		vec3 lightDir     = sampled - surfacePos;
		float lightDist   = length(lightDir);
		float lightDistSq = lightDist * lightDist;
		lightDir = normalize(lightDir);

		isShadowed = true;

		// The light has to be visible from the surface. Less than 90° between vectors.
		if (dot(payload.ffnormal, lightDir) <= 0.0 || dot(lightDir, lightNormal) >= 0.0)
			return L;

		// Shadow ray
		traceNV(TLAS, flags, 0xFF, 0, 0, 1, surfacePos, tMin, lightDir, lightDist, 1);
		
		if (!isShadowed)
		{
			float bsdfPdf = UE4Pdf(material, lightDir);
			float lightPdf = lightDistSq / (light.area.x * abs(dot(lightNormal, lightDir)));
			float cosTheta = abs(dot(payload.ffnormal, lightDir));
			vec3 F = UE4Eval(material, lightDir);

			L += (powerHeuristic(lightPdf, bsdfPdf) * F * cosTheta * emission) / lightPdf;
		}
	}

	return L;
}
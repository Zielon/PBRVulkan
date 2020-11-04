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
	BsdfSample bsdfSample;
	LightSample lightSample;

	payload.radiance += material.emission.xyz * payload.beta;

	if (interesetsEmitter(lightSample, gl_HitTNV))
	{
		vec3 Le = sampleEmitter(lightSample, payload.bsdf);
		payload.radiance += Le * payload.beta;
		payload.stop = true;
		return;
	}

	if (int(material.type) == DISNEY)
	{
		payload.specularBounce = false;
		payload.radiance += directLight(material) * payload.beta;

		bsdfSample.bsdfDir = UE4Sample(material);
		bsdfSample.pdf = UE4Pdf(material, bsdfSample.bsdfDir);

		float cosTheta = abs(dot(ffnormal, bsdfSample.bsdfDir));
		vec3 F = UE4Eval(material, bsdfSample.bsdfDir);
		payload.beta *= F * cosTheta / (bsdfSample.pdf + EPS);
	}

	if (int(material.type) == GLASS)
	{
		payload.specularBounce = true;

		bsdfSample.bsdfDir = glassSample(material);
		bsdfSample.pdf = glassPdf();

		payload.beta *= glassEval(material);
	}

	// Russian roulette
	if (max3(payload.beta) < 0.01f && payload.depth > 2)
	{
		float q = max(float(.05), 1.f - max3(payload.beta));
		if (rnd(seed) < q) 
			payload.stop = true;
		payload.beta /= 1.f - q;
	}

	payload.bsdf = bsdfSample;

	// Update a new ray path bounce direction
	payload.ray.direction = bsdfSample.bsdfDir;
	payload.ray.origin = worldPos + bsdfSample.bsdfDir * EPS;
}
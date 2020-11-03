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

	payload.radiance += material.emission.xyz * payload.throughput;

	if (interesetsEmitter(lightSample))
	{
		vec3 Le = sampleEmitter(lightSample, payload.bsdf);
		payload.radiance += Le  * payload.throughput;
		payload.stop = true;
		return;
	}

	if (material.albedo.w == 0.0) // UE4 Brdf
	{
		payload.specularBounce = false;
		payload.radiance += directLight(material) * payload.throughput;

		bsdfSample.bsdfDir = UE4Sample(material);
		bsdfSample.pdf = UE4Pdf(material, bsdfSample.bsdfDir);

		if (bsdfSample.pdf > 0.0)
		{
			float cosTheta = abs(dot(ffnormal, bsdfSample.bsdfDir));
			vec3 F = UE4Eval(material, bsdfSample.bsdfDir);
			payload.throughput *= F * cosTheta / bsdfSample.pdf;
		}
		else
		{
			payload.stop = true;
			return;
		}
	}

	// Update a new ray path bounce direction
	payload.ray.direction = bsdfSample.bsdfDir;
	payload.ray.origin = worldPos + bsdfSample.bsdfDir * EPS;

	payload.bsdf = bsdfSample;
}
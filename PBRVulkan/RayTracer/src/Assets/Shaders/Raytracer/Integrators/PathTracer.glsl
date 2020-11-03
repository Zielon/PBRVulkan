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

	if (interesetsEmitter(payload.ray, lightSample))
	{
		vec3 Le = sampleEmitter(payload.ray, lightSample, payload.bsdf);
		payload.radiance += Le  * payload.throughput;
		payload.stop = true;
		return;
	}

	if (material.albedo.w == 0.0) // UE4 Brdf
	{
		payload.specularBounce = false;
		payload.radiance += directLight(material, payload.ray) * payload.throughput;

		bsdfSample.bsdfDir = UE4Sample(payload.ray, material);
		bsdfSample.pdf = UE4Pdf(payload.ray, material, bsdfSample.bsdfDir);

		if (bsdfSample.pdf > 0.0)
			payload.throughput *= UE4Eval(payload.ray, material, bsdfSample.bsdfDir) * abs(dot(ffnormal, bsdfSample.bsdfDir)) / bsdfSample.pdf;
		else
		{
			payload.stop = true;
			return;
		}
	}

	payload.ray.direction = bsdfSample.bsdfDir;
	payload.ray.origin = worldPos + bsdfSample.bsdfDir * EPS;
	payload.bsdf = bsdfSample;
}
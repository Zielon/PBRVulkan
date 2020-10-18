/*
 * Path tracing integral
 */

{ /* Integrate */
	const vec3 barycentrics = vec3(1.0 - Hit.x - Hit.y, Hit.x, Hit.y);
	const vec3 normal = normalize(mix(v0.normal, v1.normal, v2.normal, barycentrics));
	const vec2 texCoord = mix(v0.texCoord, v1.texCoord, v2.texCoord, barycentrics);

	int textureId = material.albedoTexID; // Albedo

	if (textureId >= 0)
	{
		//Ray.color = texture(TextureSamplers[textureId], texCoord).rgb;
		Ray.color = normal;
	}
	else
	{
		Ray.color = normal;
	}
}

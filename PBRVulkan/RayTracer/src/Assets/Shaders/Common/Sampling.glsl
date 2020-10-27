/*
 * Set of different sampling functions
 */

vec3 sampleLight(Light light)
{
	float r1 = rnd(payload.seed);
	float r2 = rnd(payload.seed);
	return light.position + light.u * r1 + light.v * r2;
}
/*
 * Set of different sampling functions
 */

vec3 sampleLight(Light light, inout uint seed)
{
	float r1 = rnd(seed);
	float r2 = rnd(seed);
	return light.position + light.u * r1 + light.v * r2;
}

vec3 cosineSampleHemisphere(float u1, float u2)
{
	vec3 dir;
	float r = sqrt(u1);
	float phi = 2.0 * PI * u2;

	dir.x = r * cos(phi);
	dir.y = r * sin(phi);
	dir.z = sqrt(max(0.0, 1.0 - dir.x*dir.x - dir.y*dir.y));

	return dir;
}

vec3 uniformSampleSphere(float u1, float u2)
{
	float z = 1.0 - 2.0 * u1;
	float r = sqrt(max(0.f, 1.0 - z * z));
	float phi = 2.0 * PI * u2;

	float x = r * cos(phi);
	float y = r * sin(phi);

	return vec3(x, y, z);
}
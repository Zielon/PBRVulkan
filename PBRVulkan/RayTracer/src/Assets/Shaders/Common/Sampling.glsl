/*
 * Set of different sampling functions
 */

vec3 sampleLight(Light light)
{
	float r1 = rnd(seed);
	float r2 = rnd(seed);

	vec3 u = light.u * r1;
	vec3 v = light.v * r2;

	return light.position + u + v;
}

vec3 cosineSampleHemisphere()
{
	float r1 = rnd(seed);
	float r2 = rnd(seed);

	vec3 dir;
	float r = sqrt(r1);
	float phi = 2.0 * PI * r2;

	dir.x = r * cos(phi);
	dir.y = r * sin(phi);
	dir.z = sqrt(max(0.0, 1.0 - dir.x*dir.x - dir.y*dir.y));

	return dir;
}

vec3 uniformSampleSphere()
{
	float r1 = rnd(seed);
	float r2 = rnd(seed);

	float z = 1.0 - 2.0 * r1;
	float r = sqrt(max(0.f, 1.0 - z * z));
	float phi = 2.0 * PI * r2;

	float x = r * cos(phi);
	float y = r * sin(phi);

	return vec3(x, y, z);
}
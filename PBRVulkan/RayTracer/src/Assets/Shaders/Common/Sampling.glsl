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

float rectIntersect(in vec3 pos, in vec3 u, in vec3 v, in vec4 plane)
{
	vec3 n = vec3(plane);
	float dt = dot(gl_WorldRayDirectionNV, n);
	float t = (plane.w - dot(n, gl_WorldRayOriginNV)) / dt;
	vec3 p = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * t;
	vec3 vi = p - pos;

	if (t > EPS)
	{
		float a1 = dot(u, vi);
		if (a1 >= 0. && a1 <= 1.)
		{
			float a2 = dot(v, vi);
			if (a2 >= 0. && a2 <= 1.)
				return t;
		}
	}

	return INFINITY;
}

bool interesetsEmitter(inout LightSample lightSample, float hit)
{
	bool intersects = false;
	float t = INFINITY;
	float d = 0;

	for (uint i = 0; i < ubo.lights; ++i)
	{
		Light light = Lights[i];
		vec3 u = light.u.xyz;
		vec3 v = light.v.xyz;

		vec3 normal = normalize(cross(u, v));
		vec4 plane = vec4(normal, dot(normal, light.position));

		u *= 1.0f / dot(u, u);
		v *= 1.0f / dot(v, v);

		d = rectIntersect(light.position, u, v, plane);

		if (d < 0.)
			d = INFINITY;

		if (d < t && d < hit)
		{
			t = d;

			vec3 normal = normalize(cross(u, v));
			float cosTheta = abs(dot(-gl_WorldRayDirectionNV, normal));
			float pdf = (t * t) / (light.area.x * cosTheta);

			lightSample.emission = light.emission;
			lightSample.pdf = pdf;

			intersects = true;
		}
	}

	return intersects;
}

vec3 sampleEmitter(in LightSample lightSample, in BsdfSample bsdfSample)
{
	vec3 Le = lightSample.emission;
	return (payload.depth == 0 || payload.specularBounce) ? Le : powerHeuristic(bsdfSample.pdf, lightSample.pdf) * Le;
}
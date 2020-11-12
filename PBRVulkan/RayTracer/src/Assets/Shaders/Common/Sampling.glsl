/*
 * Set of different sampling functions
 */

void sampleAreaLight(in Light light, out LightSample lightSample)
{
	float r1 = rnd(seed);
	float r2 = rnd(seed);

	vec3 u = light.u * r1;
	vec3 v = light.v * r2;

	lightSample.normal   = normalize(cross(light.u, light.v));
	lightSample.emission = light.emission * float(ubo.lights);
	lightSample.position = light.position + u + v;
}

void sampleSphereLight(in Light light, out LightSample lightSample)
{
	vec3 position = light.position + uniformSampleSphere() * light.radius;

	lightSample.normal = normalize(position - light.position);
	lightSample.emission = light.emission * float(ubo.lights);
	lightSample.position = position;
}

LightSample sampleLight(in Light light)
{
	LightSample lightSample;

	if (light.type == AREA_LIGHT)
		sampleAreaLight(light, lightSample);	
	else
		sampleSphereLight(light, lightSample);

	return lightSample;
}

float sphereIntersect(in Light light)
{
	vec3 dir = light.position - gl_WorldRayOriginNV;
	float b = dot(dir, gl_WorldRayDirectionNV);
	float det = b * b - dot(dir, dir) + light.radius * light.radius;

	if (det < 0.0) return INFINITY;

	det = sqrt(det);

	float t1 = b - det;
	if (t1 > EPS) return t1;

	float t2 = b + det;
	if (t2 > EPS) return t2;

	return INFINITY;
}

float planeIntersect(in Light light)
{
	vec3 u = light.u;
	vec3 v = light.v;

	vec3 normal = normalize(cross(u, v));
	vec4 plane = vec4(normal, dot(normal, light.position));

	u *= 1.0 / dot(u, u);
	v *= 1.0 / dot(v, v);

	vec3 n = vec3(plane);
	float dt = dot(gl_WorldRayDirectionNV, n);
	float t = (plane.w - dot(n, gl_WorldRayOriginNV)) / dt;
	vec3 p = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * t;
	vec3 vi = p - light.position;

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

void checkAreaLightIntersection(inout float closest, float hit, in Light light, inout LightSample lightSample)
{
	float dist = planeIntersect(light);

	if (dist < 0.) dist = INFINITY;

	if (dist < closest && dist < hit)
	{
		closest = dist;

		vec3 normal = normalize(cross(light.u, light.v));
		float cosTheta = abs(dot(-gl_WorldRayDirectionNV, normal));
		float pdf = (dist * dist) / (light.area * cosTheta);

		lightSample.emission = light.emission;
		lightSample.pdf = pdf;
		lightSample.normal = normal;
	}
}


void checkSphereLightIntersection(inout float closest, float hit, in Light light, inout LightSample lightSample)
{
	float dist = sphereIntersect(light);

	if (dist < 0.) dist = INFINITY;

	if (dist < closest && dist < hit)
	{
		closest = dist;

		vec3 surfacePos = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * hit;
		vec3 normal = normalize(surfacePos - light.position);
		float pdf = (dist * dist) / light.area;

		lightSample.emission = light.emission;
		lightSample.pdf = pdf;
		lightSample.normal = normal;
	}
}

bool interesetsEmitter(inout LightSample lightSample, float hit)
{
	float closest = INFINITY;

	for (uint i = 0; i < ubo.lights; ++i)
	{
		Light light = Lights[i];

		if (light.type == AREA_LIGHT)
			checkAreaLightIntersection(closest, hit, light, lightSample);
		else
			checkSphereLightIntersection(closest, hit, light, lightSample);
	}

	return closest < INFINITY;
}

vec3 sampleEmitter(in LightSample lightSample, in BsdfSample bsdfSample)
{
	vec3 Le = lightSample.emission;
	return (payload.depth == 0 || payload.specularBounce) ? Le : powerHeuristic(bsdfSample.pdf, lightSample.pdf) * Le;
}
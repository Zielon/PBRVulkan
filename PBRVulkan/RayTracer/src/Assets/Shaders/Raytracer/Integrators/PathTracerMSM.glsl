/*
 * Path tracing Specular Manifold Sampling integral
 */

{
  const vec3 barycentrics = vec3(1.0 - Hit.x - Hit.y, Hit.x, Hit.y);
	const vec3 normal = normalize(mix(v0.normal, v1.normal, v2.normal, barycentrics));
  Ray.color = abs(normal);
}
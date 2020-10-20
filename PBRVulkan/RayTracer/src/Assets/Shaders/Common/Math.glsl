/*
 * Math helpers functions
 */

vec3 mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics) 
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

vec2 mix(vec2 a, vec2 b, vec2 c, vec3 barycentrics)
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

vec3 tone_map(in vec3 color, float limit)
{
	float luminance = 0.3 * color.x + 0.6 * color.y + 0.1 * color.z;
	return color * 1.0 / (1.0 + luminance / limit);
}
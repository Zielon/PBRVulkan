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

vec3 toneMap(in vec3 color, float limit)
{
	float luminance = 0.3 * color.x + 0.6 * color.y + 0.1 * color.z;
	return color * 1.0 / (1.0 + luminance / limit);
}

float SchlickFresnel(float u)
{
	float m = clamp(1.0 - u, 0.0, 1.0);
	float m2 = m * m;
	return m2 * m2 * m;
}

float GTR2(float NDotH, float a)
{
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0)*NDotH*NDotH;
	return a2 / (PI * t*t);
}

float GGX(float NDotv, float alphaG)
{
	float a = alphaG * alphaG;
	float b = NDotv * NDotv;
	return 1.0 / (NDotv + sqrt(a + b - a * b));
}

float powerHeuristic(float a, float b)
{
	float t = a * a;
	return t / (b * b + t);
}
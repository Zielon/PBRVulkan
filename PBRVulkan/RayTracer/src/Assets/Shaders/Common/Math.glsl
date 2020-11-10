/*
 * Math helpers functions
 */

/*
 * Linear interpolate between 3 vectors
 */
vec3 mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics) 
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

/*
 * Linear interpolate between 2 vectors
 */
vec2 mix(vec2 a, vec2 b, vec2 c, vec3 barycentrics)
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

 /*
  * Encode color from HDR space to LDR. Tone mapping.
  */
vec3 toneMap(in vec3 rgb, float limit)
{
	float luminance = 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
	return rgb / (1.0 + luminance / limit);
}

/*
 * Gamma correction. Gamut mapping.
 */
vec3 gammaCorrection(in vec3 ldr)
{
	return pow(ldr, vec3(1.0 / 2.2));
}

/*
 * Calculates local coordinate frame for a given normal
 */
mat3 localFrame(in vec3 normal)
{
	vec3 up       = abs(normal.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = cross(normal, tangentX);
	return mat3(tangentX, tangentY, normal);
}

 /*
  * Schlick's approximation of the specular reflection coefficient R
  * (1 - cosTheta)^5
  */
float SchlickFresnel(float u)
{
	float m = clamp(1.0 - u, 0.0, 1.0);
	return m * m * m * m * m; // power of 5
}

 /*
  * Generalized-Trowbridge-Reitz (D)
  * Describes differential area of microfacets for the surface normal
  */
float GTR2(float NDotH, float a)
{
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0)*NDotH*NDotH;
	return a2 / (PI * t*t);
}

 /*
  * The masking shadowing function Smith for GGX noraml distribution (G)
  */
float SmithGGX(float NDotv, float alphaG)	
{
	float a = alphaG * alphaG;
	float b = NDotv * NDotv;
	return 1.0 / (NDotv + sqrt(a + b - a * b));
}

 /*
  * Power heuristic often reduces variance even further for multiple importance sampling
  * Chapter 13.10.1 of pbrbook
  */
float powerHeuristic(float a, float b)
{
	float t = a * a;
	return t / (b * b + t);
}

/*
 * Max component of a vector3
 */
float max3(vec3 v) 
{
	return max (max (v.x, v.y), v.z);
}
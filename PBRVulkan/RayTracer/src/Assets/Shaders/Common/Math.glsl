/*
 * Math helpers functions
 */

/*
 * Linear interpolate between 3 vectors
 * @param vector a
 * @param vector b
 * @param vector c
 * @param barycentric coordinates (interpolation weights)
 * @returns 3D interpolated values
 */
vec3 mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics) 
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

/*
 * Linear interpolate between 2 vectors
 * @param vector a
 * @param vector b
 * @param barycentric coordinates (interpolation weights)
 * @returns 2D interpolated values
 */
vec2 mix(vec2 a, vec2 b, vec2 c, vec3 barycentrics)
{
	return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

 /*
  * Encode color from HDR space to LDR
  * @param color Color vector
  * @param limit Scaling limit
  * @returns LDR color encoding
  */
vec3 toneMap(in vec3 hdr, float limit)
{
	float luminance = 0.299 * hdr.x + 0.587 * hdr.y + 0.114 * hdr.z;
	return hdr / (1.0 + luminance / limit);
}

/*
 * Gamma correction 
 * @param color Color vector
 * @returns LDR color value encoding
 */
vec3 gammaCorrection(in vec3 ldr)
{
	return pow(ldr, vec3(1.0 / 2.2));
}

/*
 * Calculates local coordinate frame for a given normal
 * @param normal Normal vector 
 */
mat3 localFrame(in vec3 normal)
{
	vec3 up = abs(normal.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 x  = normalize(cross(up, normal));
	vec3 y  = cross(normal, x);
	return mat3(x, y, normal);
}

 /*
  *
  */
float SchlickFresnel(float u)
{
	float m = clamp(1.0 - u, 0.0, 1.0);
	float m2 = m * m;
	return m2 * m2 * m;
}

 /*
  *
  */
float GTR2(float NDotH, float a)
{
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0)*NDotH*NDotH;
	return a2 / (PI * t*t);
}

 /*
  *
  */
float GGX(float NDotv, float alphaG)
{
	float a = alphaG * alphaG;
	float b = NDotv * NDotv;
	return 1.0 / (NDotv + sqrt(a + b - a * b));
}

 /*
  *
  */
float powerHeuristic(float a, float b)
{
	float t = a * a;
	return t / (b * b + t);
}
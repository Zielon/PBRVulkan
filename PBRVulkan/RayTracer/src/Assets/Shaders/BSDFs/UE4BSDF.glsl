/*
 *	Based on    https://github.com/knightcrawler25/GLSL-PathTracer
 *  UE4 SIGGAPH https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
 */

float UE4Pdf(in Material material, in vec3 bsdfDir)
{
	vec3 N = payload.ffnormal;
	vec3 V = -gl_WorldRayDirectionEXT;
	vec3 L = bsdfDir;

	float specularAlpha = max(0.001, material.roughness);

	float diffuseRatio = 0.5 * (1.0 - material.metallic);
	float specularRatio = 1.0 - diffuseRatio;

	vec3 halfVec = normalize(L + V);

	float cosTheta = abs(dot(halfVec, N));
	float pdfGTR2 = GTR2(cosTheta, specularAlpha) * cosTheta;

	// calculate diffuse and specular pdfs and mix ratio
	float pdfSpec = pdfGTR2 / (4.0 * abs(dot(L, halfVec)));
	float pdfDiff = abs(dot(L, N)) * INV_PI;

	// weight pdfs according to ratios
	return diffuseRatio * pdfDiff + specularRatio * pdfSpec;
}

vec3 UE4Sample(in Material material)
{
	vec3 N = payload.ffnormal;
	vec3 V = -gl_WorldRayDirectionEXT;

	vec3 dir;

	float probability = rnd(seed);
	float diffuseRatio = 0.5 * (1.0 - material.metallic);

	mat3 frame = localFrame(N);

	if (probability < diffuseRatio) // sample diffuse
	{
		dir = frame * cosineSampleHemisphere();
	}
	else
	{
		float a = max(0.001, material.roughness);
		float r1 = rnd(seed);
		float r2 = rnd(seed);
		float phi = r1 * TWO_PI;

		float cosTheta = sqrt((1.0 - r2) / (1.0 + (a*a - 1.0) *r2));
		float sinTheta = clamp(sqrt(1.0 - (cosTheta * cosTheta)), 0.0, 1.0);
		float sinPhi = sin(phi);
		float cosPhi = cos(phi);

		vec3 halfVec = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
		halfVec = frame * halfVec;

		dir = 2.0 * dot(V, halfVec) * halfVec - V;
	}

	return dir;
}

vec3 UE4Eval(in Material material, in vec3 bsdfDir)
{
	vec3 N = payload.ffnormal;
	vec3 V = -gl_WorldRayDirectionEXT;
	vec3 L = bsdfDir;
	vec3 albedo = material.albedo.xyz;

	float NDotL = dot(N, L);
	float NDotV = dot(N, V);

	if (NDotL <= 0.0 || NDotV <= 0.0)
		return vec3(0.0);

	vec3 H = normalize(L + V);
	float NDotH = dot(N, H);
	float LDotH = dot(L, H);

	// Specular
	float specular = 0.5;
	vec3 specularCol = mix(vec3(1.0) * 0.08 * specular, albedo, material.metallic);
	float a = max(0.001, material.roughness);
	float D = GTR2(NDotH, a);
	float FH = SchlickFresnel(LDotH);
	vec3 F = mix(specularCol, vec3(1.0), FH);
	float roughg = (material.roughness * 0.5 + 0.5);
	roughg = roughg * roughg;

	float G = SmithGGX(NDotL, roughg) * SmithGGX(NDotV, roughg);

	// Diffuse + Specular components
	return (INV_PI * albedo) * (1.0 - material.metallic) + F * D * G;
}

float glassPdf()
{
	return 1.0;
}

vec3 glassSample(in Material material)
{
	float n1 = 1.0;
	float n2 = material.ior;
	float R0 = (n1 - n2) / (n1 + n2);
	R0 *= R0;
	float theta = dot(-gl_WorldRayDirectionEXT, payload.ffnormal);
	float prob = R0 + (1. - R0) * SchlickFresnel(theta);
	float eta = payload.eta;
	vec3 refractDir = normalize(refract(gl_WorldRayDirectionEXT, payload.ffnormal, eta));
	float cos2t = 1.0 - eta * eta * (1.0 - theta * theta);

	vec3 dir;

	// Reflection
	if (cos2t < 0.0 || rnd(seed) < prob)
	{
		dir = normalize(reflect(gl_WorldRayDirectionEXT, payload.ffnormal));
	}
	// Transmission
	else
	{
		dir = refractDir;
	}
	
	return dir;
}

vec3 glassEval(in Material material)
{
	return material.albedo.xyz;
}
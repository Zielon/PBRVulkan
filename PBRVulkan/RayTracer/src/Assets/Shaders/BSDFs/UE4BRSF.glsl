/*
 *
 */

float UE4Pdf(in Material material, in vec3 bsdfDir)
{
	vec3 N = payload.ffnormal;
	vec3 V = -gl_WorldRayDirectionNV;
	vec3 L = bsdfDir;

	float specularAlpha = max(0.001, material.roughness);

	float diffuseRatio = 0.5 * (1.0 - material.metallic);
	float specularRatio = 1.0 - diffuseRatio;

	vec3 halfVec = normalize(L + V);

	float cosTheta = abs(dot(halfVec, N));
	float pdfGTR2 = GTR2(cosTheta, specularAlpha) * cosTheta;

	// calculate diffuse and specular pdfs and mix ratio
	float pdfSpec = pdfGTR2 / (4.0 * abs(dot(L, halfVec)));
	float pdfDiff = abs(dot(L, N)) * (1.0 / PI);

	// weight pdfs according to ratios
	return diffuseRatio * pdfDiff + specularRatio * pdfSpec;
}

vec3 UE4Sample(in Material material)
{
	vec3 N = payload.ffnormal;
	vec3 V = -gl_WorldRayDirectionNV;

	vec3 dir;

	float probability = rnd(seed);
	float diffuseRatio = 0.5 * (1.0 - material.metallic);

	float r1 = rnd(seed);
	float r2 = rnd(seed);

	mat3 frame = localFrame(N);

	if (probability < diffuseRatio) // sample diffuse
	{
		dir = frame * cosineSampleHemisphere();
	}
	else
	{
		float a = max(0.001, material.roughness);

		float phi = r1 * 2.0 * PI;

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
	vec3 V = -gl_WorldRayDirectionNV;
	vec3 L = bsdfDir;

	float NDotL = dot(N, L);
	float NDotV = dot(N, V);

	if (NDotL <= 0.0 || NDotV <= 0.0)
		return vec3(0.0);

	vec3 H = normalize(L + V);
	float NDotH = dot(N, H);
	float LDotH = dot(L, H);

	// specular	
	float specular = 0.5;
	vec3 specularCol = mix(vec3(1.0) * 0.08 * specular, material.albedo.xyz, material.metallic);
	float a = max(0.001, material.roughness);
	float Ds = GTR2(NDotH, a);
	float FH = SchlickFresnel(LDotH);
	vec3 Fs = mix(specularCol, vec3(1.0), FH);
	float roughg = (material.roughness * 0.5 + 0.5);
	roughg = roughg * roughg;

	float Gs = GGX(NDotL, roughg) * GGX(NDotV, roughg);

	return (material.albedo.xyz / PI) * (1.0 - material.metallic) + Gs * Fs * Ds;
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
	float theta = dot(-gl_WorldRayDirectionNV, payload.ffnormal);
	float prob = R0 + (1. - R0) * SchlickFresnel(theta);
	vec3 dir;

	//vec3 transmittance = vec3(1.0);
	//vec3 extinction = -log(vec3(0.1, 0.1, 0.908));
	//vec3 extinction = -log(vec3(0.905, 0.63, 0.3));

	float eta = dot(payload.normal, payload.ffnormal) > 0.0 ? (n1 / n2) : (n2 / n1);
	vec3 transDir = normalize(refract(gl_WorldRayDirectionNV, payload.ffnormal, eta));
	float cos2t = 1.0 - eta * eta * (1.0 - theta * theta);

	//if(dot(-ray.direction, state.normal) <= 0.0)
	//	transmittance = exp(-extinction * state.hitDist * 100.0);

	if (cos2t < 0.0 || rnd(seed) < prob) // Reflection
	{
		dir = normalize(reflect(gl_WorldRayDirectionNV, payload.ffnormal));
	}
	else  // Transmission
	{
		dir = transDir;
	}
	//state.mat.albedo.xyz = transmittance;
	return dir;
}

vec3 glassEval(in Material material)
{
	return material.albedo.xyz;
}
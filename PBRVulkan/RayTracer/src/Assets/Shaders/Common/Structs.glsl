/*
 *  Global structure used in shaders
 */

#define PI        3.14159265358979323846
#define TWO_PI    6.2831853071795864
#define INV_PI    0.31830988618379067154
#define INV_2PI   0.15915494309189533577
#define EPS       0.001
#define INFINITY  1000000.0

uint seed = 0;

struct Material 
{ 
	vec4 albedo;
	vec4 emission;
	// Parameters
	float metallic;
	float roughness;
	float ior;
	float transmittance;
	// Textures
	int albedoTexID;
	int metallicRoughnessTexID;
	int normalmapTexID;
	int heightmapTexID;
};

struct Light 
{ 
	vec3 position; 
	vec3 emission; 
	vec3 u; 
	vec3 v; 
	vec3 radiusAreaType; 
};

struct Uniform
{
	mat4 view;
	mat4 proj;
	vec3 direction;
	vec2 random;
	uint lights;
	bool useHDR;
	uint spp;
	uint maxDepth;
	uint frame;
	float aperture;
	float focalDistance;
	float hdrResolution;
	float AORayLength;
};

struct RayPayload
{
	vec3 radiance;
	vec3 throughput;
	vec3 worldPos;
	vec3 normal;
	uint depth;
};

struct BsdfSample
{
	vec3 bsdfDir; 
	float pdf; 
};
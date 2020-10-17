/*
 *  Global structure used in shaders
 */

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

struct Uniform
{
	mat4 view;
	mat4 proj;
	vec3 direction;
};

struct RayPayload
{
	vec3 color;
};

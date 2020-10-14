// Global structure used in shaders

struct Material 
{ 
    vec4 albedo; 
    vec4 emission; 
    vec4 param; 
    ivec4 texIDs; 
};

struct Uniform
{
    mat4 view;
    mat4 proj;
    vec3 direction;
};
#extension GL_EXT_control_flow_attributes : require

float rand(inout vec2 seed, vec2 random)
{
	seed -= vec2(random.x * random.y);
	return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}
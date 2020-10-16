/*
 * General vertex structure
 * For reference check Geometry/Vertex.h files
 */

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    int materialIndex;
};

Vertex unpack(uint index)
{
	const uint vertexSize = 9;
	const uint offset = index * vertexSize;
	
	Vertex vertex;
	
	vertex.position = vec3(Vertices[offset + 0], Vertices[offset + 1], Vertices[offset + 2]);
	vertex.normal = vec3(Vertices[offset + 3], Vertices[offset + 4], Vertices[offset + 5]);
	vertex.texCoord = vec2(Vertices[offset + 6], Vertices[offset + 7]);
	vertex.materialIndex = floatBitsToInt(Vertices[offset + 8]);

	return vertex;
}

#version 410 core
uniform vec2 m_v2TerrainSize;
uniform mat4 mvp_matrix;
vec4 vertices[4] = vec4[4](vec4(-0.5, 0.0, -0.5, 1.0),
	vec4(0.5, 0.0, -0.5, 1.0),
	vec4(0.5, 0.0, 0.5, 1.0),
	vec4(-0.5, 0.0, 0.5, 1.0));

void main(void)
{
	vec4 v4Position = vertices[gl_VertexID];
	v4Position.x *= m_v2TerrainSize.x;
	v4Position.z *= m_v2TerrainSize.y;
	gl_Position = mvp_matrix * v4Position;
}
#version 410 core
out VS_OUT
{
	vec2 m_v2TextureCoord;
} vs_out;
vec2[4] VertexArray = vec2[4](vec2(-1, 1), vec2(-1, -1), vec2(1, -1), vec2(1, 1));
vec2[4] TextureCoordArray = vec2[4](vec2(0, 1), vec2(0, 0), vec2(1, 0), vec2(1, 1));
void main(void)
{
	gl_Position = vec4(VertexArray[gl_VertexID], 1, 1);
	vs_out.m_v2TextureCoord = TextureCoordArray[gl_VertexID];
}
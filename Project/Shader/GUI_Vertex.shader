#version 410 core
out VS_OUT
{
	vec2 texcoord;
} vs_out;
vec2 m_v2PositionArray[4] = { vec2(-0.5,0.5),vec2(-0.5,-0.5), vec2(0.5,-0.5), vec2(0.5,0.5) };
vec2 m_v2TextureCoordArray[4] = { vec2(0,1),vec2(0,0), vec2(1,0), vec2(1,1) };
uniform vec2 m_v2Origin = vec2(0.f, 0.f);
uniform vec2 m_v2Size;
void main(void)
{
	vec2 v2Point = vec2(0, 0);
	gl_Position = vec4(m_v2Origin + m_v2PositionArray[gl_VertexID] * m_v2Size, 1, 1);
	//gl_Position = vec4(position, 1.0, 1.0);
	vs_out.texcoord = m_v2TextureCoordArray[gl_VertexID];
	//vs_out.texcoord = texcoord;
}
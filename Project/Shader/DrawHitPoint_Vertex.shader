#version 410 core

vec2[4] VertexArray = vec2[4](vec2(1, -1), vec2(-1, -1), vec2(-1, 1), vec2(1, 1));
vec2[4] TextureCoordArray = vec2[4](vec2(1, 0), vec2(0, 0), vec2(0, 1), vec2(1, 1));

out VS_OUT
{
	vec2 texcoord;
} vs_out;
void main(void)
{
	gl_Position = vec4(VertexArray[gl_VertexID], 1.0, 1.0);
	vs_out.texcoord = TextureCoordArray[gl_VertexID];
}
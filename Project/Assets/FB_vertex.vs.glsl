#version 410 core

out VS_OUT
{
	vec2 texcoord;
} vs_out;

const vec2 window_position[] = vec2[](
	vec2( 1.0f,-1.0f),
	vec2(-1.0f,-1.0f),
	vec2(-1.0f, 1.0f),
	vec2( 1.0f, 1.0f)
	);
const vec2 tex_coord[] = vec2[](
	vec2(1.0f,0.0f),
	vec2(0.0f,0.0f),
	vec2(0.0f,1.0f),
	vec2(1.0f,1.0f)
	);

void main()
{
	gl_Position = vec4(vec2(0.5) * window_position[gl_VertexID],1.0,1.0);
	vs_out.texcoord = tex_coord[gl_VertexID];
}
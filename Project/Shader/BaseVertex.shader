#version 410 core
uniform mat4 mvp_matrix;
layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 iv2tex_coord;

out VS_OUT
{
	vec2 texcoord;
} fs_out;
void main(void)
{
	gl_Position = mvp_matrix * vec4(iv3vertex, 1);
	fs_out.texcoord = iv2tex_coord;
}
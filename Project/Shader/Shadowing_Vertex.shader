#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 iv2tex_coord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 shadow_matrix;
uniform vec3 light_pos;

out VS_OUT
{
	vec2 TextureCoord;
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;
void main(void)
{
	vec3 v3LightPosition = mat3(mv_matrix) * light_pos;
	vs_out.TextureCoord = iv2tex_coord;
	// Calculate view-space coordinate
	vec4 P = mv_matrix * vec4(position, 1);
	// Calculate normal in view-space
	vs_out.N = mat3(mv_matrix) * normal;
	// Calculate light vector
	vs_out.L = v3LightPosition - P.xyz;
	// Calculate view vector
	vs_out.V = -P.xyz;
	// Light-space coordinates
	vs_out.shadow_coord = shadow_matrix * vec4(position, 1);
	// Calculate the clip-space position of each vertex
	gl_Position = proj_matrix * P;
}
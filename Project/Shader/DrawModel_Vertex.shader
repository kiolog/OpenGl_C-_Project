#version 410 core

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 iv2tex_coord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 shadow_matrix;
uniform vec3 m_v3LightPosition;

out VS_OUT
{
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

void main(void)
{
	vec3 v3LightPosition = mat3(mv_matrix) * m_v3LightPosition;
	// Calculate view-space coordinate
	vec4 P = mv_matrix * vec4(iv3vertex, 1);
	// Calculate normal in view-space
	vs_out.N = mat3(mv_matrix) * normal;
	// Calculate light vector
	vs_out.L = v3LightPosition - P.xyz;
	// Calculate view vector
	vs_out.V = -P.xyz;
	// Calculate the clip-space position of each vertex
	vs_out.shadow_coord = shadow_matrix * vec4(iv3vertex, 1);
	gl_Position = proj_matrix * P;
}
#version 410 core
layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 iv2tex_coord;

uniform mat4 view_matrix;
uniform mat4 shadow_matrix;
uniform mat4 model_matrix;
uniform mat4 proj_matrix;
uniform vec4 m_v4Plane = vec4(0, 1, 0, -1);
uniform vec3 m_v3LightPosition = vec3(-1000,-1000,-1000);
uniform vec3 m_v3EyePosition;

out VS_OUT
{
	vec2 m_v2TextureCoord;
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 H;

} vs_out;
void main(void)
{
	mat4 mv_matrix = view_matrix * model_matrix;
	gl_Position = proj_matrix * mv_matrix * vec4(iv3vertex, 1);
	vs_out.m_v2TextureCoord = iv2tex_coord;

	vec4 v4WorldPosition = model_matrix * vec4(iv3vertex, 1);
	gl_ClipDistance[0] = dot(v4WorldPosition, m_v4Plane);
	vs_out.shadow_coord = shadow_matrix * v4WorldPosition;

	vec3 v3LightPosition = mat3(view_matrix) * m_v3LightPosition;
	vec3 v3EyePosition = mat3(view_matrix) * m_v3EyePosition;
	// Calculate view-space coordinate
	vec4 P = mv_matrix * vec4(iv3vertex, 1);
	// Calculate normal in view-space
	vs_out.N = mat3(mv_matrix) * normal;
	// Calculate light vector
	vs_out.L = v3LightPosition - P.xyz;
	// Calculate view vector
	vec3 V = v3EyePosition -P.xyz;
	vs_out.H = normalize(vs_out.L + V);
	// Light-space coordinates
}
#version 410

uniform mat4 mvp_matrix;
uniform mat4 shadow_matrix;

vec3[4] VertexArray = vec3[4](vec3(-50, -13, -50), vec3(50, -13, -50), vec3(50, -13, 50), vec3(-50, -13, 50));
out VS_OUT
{
	vec4 shadow_coord;
} vs_out;
void main(void)
{
	vec3 v3Position = VertexArray[gl_VertexID];
	vec4 v4VertexPosition = mvp_matrix * vec4(v3Position, 1);
	vs_out.shadow_coord = shadow_matrix * vec4(v3Position, 1);
	gl_Position = v4VertexPosition;
}
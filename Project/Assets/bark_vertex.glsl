#version 410

layout(location = 0) in vec3 iv3vertex;
//layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 um4m;
uniform mat4 um4v;
uniform mat4 um4p;
vec3 position[17] = 
{
	vec3(-60, 36, 123.2),
	vec3(-44.8, 38.5, 152.8),
	vec3(-60, 32, 184.9),
	vec3(-45.1, 24.5, 215),

	vec3(-90.7, 36, 124.9),
	vec3(-75, 38.5, 152.9),
	vec3(-90.2, 31.5, 185.2),
	vec3(-75.1, 24, 215.1),

	vec3(-104.8, 32, 153.1),
	vec3(-120.7, 26, 184.9),
	vec3(-105, 25, 215.1),

	vec3(-135, 28, 152.9),
	vec3(-150.1, 25, 184.9),
	vec3(-134.9, 24, 215.1),

	vec3(-165, 20, 150.9),
	vec3(-180.8, 22, 185.1),
	vec3(-164.9, 21, 205.1),
};

uniform vec3 light_pos = vec3(50.0f, 10.0f, 50.0f);
out vec3 vv3normal;
out VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;
mat4 translate(mat4 m, vec3 v)
{
	mat4 Result = m;
	Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}
mat4 ini = mat4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
void main()
{
	mat4 move = translate(ini, position[gl_InstanceID]);
    gl_Position = um4p * um4v * move * um4m * vec4(iv3vertex, 1.0);
    //vertexData.texcoord = iv2tex_coord;
	vec4 P = um4v * um4m * vec4(iv3vertex, 1.0);

	vertexData.N = mat3(um4v * um4m) * iv3normal;
	vertexData.L = light_pos - P.xyz;
	vertexData.H = -P.xyz;
	vv3normal = mat3(um4m) * iv3normal;
}
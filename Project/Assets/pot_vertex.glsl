#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 um4m;
uniform mat4 um4v;
uniform mat4 um4p;
vec3 position[10] = 
{
	vec3(-103.0f, 5.0f, -51.0f),
	vec3(-103.0f, 5.0f, -32.0f),
	vec3(-103.0f, 5.0f, -110.0f),
	vec3(-103.0f, 5.0f, -95.0f),
	vec3(-121.0f, 5.0f, -134.0f),
	vec3(-136.0f, 5.0f, -134.0f),
	vec3(-160.0f, 4.0f, -110.0f),
	vec3(-160.0f, 5.0f, -95.0f),
	vec3(-160.0f, 5.0f, -51.0f),
	vec3(-160.0f, 3.0f, -33.0f),
};
//float angle[5] = {90.0f, 90.0f, 0.0f, 90.0f, 90.0f};
uniform vec3 light_pos = vec3(1, 1, 1);
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
mat4 rotate(mat4 m, float angle, vec3 v)
{
	float a = angle;
	float c = cos(a);
	float s = sin(a);

	vec3 axis = (normalize(v));
	vec3 temp = ((float(1) - c) * axis);

	mat4 Rotate ;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	mat4 Result ;
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];
	return Result;
}
mat4 ini = mat4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
void main()
{
	mat4 move = translate(ini, position[gl_InstanceID]);
	//mat4 rot = rotate(ini, radians(angle[gl_InstanceID]), vec3(0.0, 1.0, 0.0));

    gl_Position = um4p * um4v * move * um4m * vec4(iv3vertex, 1.0);
    vertexData.texcoord = iv2tex_coord;
	vec4 P = um4v * um4m * vec4(iv3vertex, 1.0);

	vertexData.N = mat3(um4v * um4m) * iv3normal;
	vertexData.L = light_pos - P.xyz;
	vertexData.H = -P.xyz;
	vec4 temp = rotate(ini, radians(-90), vec3(1.0, 0.0, 0.0)) * vec4(iv3normal, 1.0);
	vv3normal = temp.rgb;
}
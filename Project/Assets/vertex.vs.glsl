#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;
layout(location = 3) in vec3 iv3tangent;
layout(location = 4) in vec3 iv3bitangent;

uniform mat4 um4v;
uniform mat4 um4m;
uniform mat4 um4p;


out VertexData
{
	vec3 normal;
	vec3 fragPos;
    vec2 texcoord;
	mat3 TBN;
} vertexData;


void main()
{
	gl_Position = um4p *um4v * um4m * vec4(iv3vertex, 1.0);
    vertexData.texcoord = iv2tex_coord;
	vertexData.normal =  normalize((vec4(iv3normal,0)).xyz);
	vertexData.fragPos = vec3(um4m * vec4(iv3vertex,1.0f));

	vec3 T = normalize(vec3(um4m * vec4(iv3tangent, 0.0)));
	vec3 B = normalize(vec3(um4m * vec4(iv3bitangent, 0.0)));
	vec3 N = normalize(vec3(um4m * vec4(iv3normal, 0.0)));
	vertexData.TBN = mat3(T,B,N);
}
#version 410

uniform mat4 um4mv;
uniform mat4 um4p;

out VertrxData {
	vec3 texcoord;
} vertexData;

void main () 
{
	vec4 position = um4p * um4mv * vec4(vertex, 1);
	gl_Position = position.xyww;
	vertexData.texcoord = vertex;
}

#version 410

in VertrxData {
	vec3 texcoord;
} vertexData;

uniform samplerCube tex;

void main () 
{
	fragColor = texture(tex, vertexData.texcoord);
}
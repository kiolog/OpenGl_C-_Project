#version 410 core

uniform sampler2D tex;

out vec4 color;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main(){
	color = texture(tex, fs_in.texcoord);
}
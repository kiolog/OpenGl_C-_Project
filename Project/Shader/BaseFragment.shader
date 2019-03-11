#version 410 core
uniform sampler2D tex;
out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
void main(void)
{
	FragColor = texture(tex, fs_in.texcoord);
	//FragColor = vec4(0, 0, 1, 1);
	//fragmentdepth = 0.3f;
	//FragColor = vec4(1, 1, 1, 1);
}
#version 420
layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D shadow_tex;
out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
/*uniform vec2 m_v2ImageSize = vec2(1024, 768);
uniform vec2 m_fUIPosition;
uniform float m_fTime = 0;
uniform vec2 m_iApple;
int nbins = 8;*/
void main(void)
{
	float depthValue = texture(shadow_tex, fs_in.texcoord).r * 500;
	FragColor = texture(tex, fs_in.texcoord);
	//FragColor = vec4(vec3(depthValue), 1.0);
	//FragColor = vec4(1, 0, 0, 1);
}
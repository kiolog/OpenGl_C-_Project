#version 420 core
layout(binding = 0) uniform sampler2D m_Texture;
out vec4 FragColor;
in VS_OUT
{
	vec2 m_v2TextureCoord;
} fs_in;

void main(void)
{
	FragColor = texture(m_Texture, fs_in.m_v2TextureCoord);
}
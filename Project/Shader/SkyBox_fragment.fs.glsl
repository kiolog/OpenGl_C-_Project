#version 410

layout(location = 0) out vec4 fragColor;

in vec3 m_v3TextureCoord;

uniform samplerCube m_CubeMap;

void main()
{
	fragColor = texture(m_CubeMap, m_v3TextureCoord);
	//fragColor = vec4(1, 0, 0,1);
}
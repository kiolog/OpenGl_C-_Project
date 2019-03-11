#version 410

layout(location = 0) in vec3 iv3vertex;

uniform mat4 mvp;

out vec3 m_v3TextureCoord;

void main()
{
	gl_Position = mvp * vec4(iv3vertex, 1.0);
	m_v3TextureCoord = iv3vertex;
}
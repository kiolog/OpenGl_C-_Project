#version 410
uniform float m_fScalar = 0.001;
out vec4 FragColor;
flat in vec3 m_v3WorldPosition;
void main(void){
	FragColor = vec4((m_v3WorldPosition + m_fScalar * 0.5) / m_fScalar, 1);
}
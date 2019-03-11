#version 410

layout (location = 0) in vec3 position;

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
flat out vec3 m_v3WorldPosition;

void main(void){
	m_v3WorldPosition = (model_matrix * vec4(position, 1)).xyz;
	gl_Position = mvp_matrix * vec4(position, 1);
}
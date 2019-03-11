#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out VS_OUT{
	flat vec3 surfaceNormal;
	flat vec4 color;
	vec3 toLightVector;
	vec3 toCameraVector;
	vec4 c;
	vec4 shadow_coord;
} pass ;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition;
uniform mat4 modelMatrix;
uniform vec4 m_v4Plane = vec4(0, 1, 0, -1);
uniform mat4 shadow_matrix;

void main(void){
	mat4 mvMatrix = viewMatrix * modelMatrix;
	vec4 viewPosition = mvMatrix * vec4(position,1.0);
	gl_Position = projectionMatrix *  viewPosition;
	
	pass.surfaceNormal  = mat3(mvMatrix) * normal;
	pass.toLightVector  = (viewMatrix*vec4(lightPosition,1)).xyz - viewPosition.xyz;
	pass.toCameraVector = -viewPosition.xyz;
	pass.c =  vec4(vec3(position.y) / 50,1.0);
	pass.color = color;//vec3(mvMatrix * vec4(normal,0.0));
	vec4 v4WorldPosition = modelMatrix * vec4(position, 1);
	pass.shadow_coord = shadow_matrix * v4WorldPosition;
	gl_ClipDistance[0] = dot(v4WorldPosition, m_v4Plane);
	// pass.c = vec4(normalize(vec3(pass.surfaceNormal.r,0,pass.surfaceNormal.b)),1);
}
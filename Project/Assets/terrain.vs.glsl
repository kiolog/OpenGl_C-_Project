#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out VS_OUT{
	flat vec3 surfaceNormal;
	flat vec4 color;
	flat vec3 toLightVector;
	flat vec3 toCameraVector;
	flat float snow;
} pass ;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec4 m_v4Plane = vec4(0, 1, 0, -1);
uniform vec3 lightPosition;
uniform float winterFraction;

float getSnowFraction(void){
	float deg2vertical = degrees(acos(dot(normalize(normal), vec3(0,1,0))));
	float magnitude = 80;
	if(deg2vertical < winterFraction * magnitude)
		deg2vertical = deg2vertical  / (winterFraction * magnitude);
	else 
		deg2vertical = 180 ;
	return (1 -  deg2vertical/180.0f) * clamp(0.3 + winterFraction, 0, 1);
}


void main(void){
	pass.snow = getSnowFraction();
	if(position.y <= -8)
		pass.snow = pass.snow *  winterFraction * 0.4;

	mat4 mvMatrix = viewMatrix * modelMatrix;
	vec3 pos = vec3(position.x ,position.y + pass.snow * 2 * color.w, position.z);
	vec4 viewPosition = mvMatrix * vec4(pos,1.0);
	gl_Position = projectionMatrix *  viewPosition;
	
	pass.surfaceNormal  = mat3(mvMatrix) * normal;
	pass.toLightVector  = (viewMatrix*vec4(lightPosition,1)).xyz - viewPosition.xyz;
	pass.toCameraVector = -viewPosition.xyz;
	pass.color = color;

	vec4 v4WorldPosition = modelMatrix * vec4(position, 1);
	gl_ClipDistance[0] = dot(v4WorldPosition, m_v4Plane);
}
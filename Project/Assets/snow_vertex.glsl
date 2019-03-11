#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2texcoord;
layout(location = 2) in vec3 iv3normal;
//out vec2 texcoord;
uniform mat4 um4m;
uniform mat4 um4p;
uniform mat4 um4v;

void main()
{
	gl_Position = um4p * um4v * um4m * vec4(iv3vertex, 1.0);
	//texcoord = iv2texcoord;
	
	const vec4 vertices[] = vec4[]( vec4( 1.0,  -1.0, 1.0, 1.0),
									vec4(-1.0,  -1.0, 1.0, 1.0), 
									vec4( 1.0,  1.0,  1.0, 1.0), 
									vec4(-1.0,  1.0,  1.0, 1.0) );
	//gl_Position =um4p * um4v * um4m * vertices[gl_VertexID];
}
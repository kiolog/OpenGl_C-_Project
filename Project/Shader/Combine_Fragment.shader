#version 420
layout(binding = 0) uniform sampler2D Obj_Texture;
layout(binding = 1) uniform sampler2D NoObj_Texture;
layout(binding = 2) uniform sampler2D Scene_Texture;

uniform int m_iSelectTexture = 0;

out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
void main(void)
{
	vec3 ObjColor = texture(Obj_Texture, fs_in.texcoord).xyz;
	vec3 NoObjColor = texture(NoObj_Texture, fs_in.texcoord).xyz;
	vec3 SceneColor = texture(Scene_Texture, fs_in.texcoord).xyz;

	switch (m_iSelectTexture) {
	case 0:
		FragColor = vec4(ObjColor, 1);
		break;
	case 1:
		FragColor = vec4(NoObjColor, 1);
		break;
	case 2:
		FragColor = vec4(SceneColor, 1);
		break;
	case 3:
		FragColor = vec4(SceneColor + (ObjColor - NoObjColor), 1);
		break;
	}

	
	//FragColor = vec4(1, 1, 1, 1);
}
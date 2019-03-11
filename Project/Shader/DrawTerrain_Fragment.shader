#version 420 core                                                          
                                                                           
out vec4 FragColor;                                                            
                                                                           
layout(binding = 0) uniform sampler2D tex_displacement;
layout(binding = 1) uniform sampler2D m_Texture;
layout(binding = 2) uniform sampler2D shadow_tex;      

uniform float m_fTime;
in VS_OUT{
	vec2 tc;
	vec4 shadow_coord;
} fs_in;
float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2Coord = fs_in.shadow_coord.xy / fs_in.shadow_coord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(shadow_tex, fs_in.shadow_coord.xy / fs_in.shadow_coord.w).r < (fs_in.shadow_coord.z / fs_in.shadow_coord.w)) {
			ReturnValue = 0.3f;
		}
	}
	return ReturnValue;
}
void main(void)                                                            
{   
	FragColor = vec4(texture(m_Texture, fs_in.tc).xyz * CalculateShadowValue(), 1);
	float fValue = pow(texture(shadow_tex, fs_in.tc).r,100);
	//FragColor = vec4(fValue, fValue, fValue, 1);
}                                                                          
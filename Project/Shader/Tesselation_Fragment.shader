#version 420 core                                                          
                                                                           
out vec4 FragColor;                                                            
                                                                           
layout(binding = 0) uniform sampler2D tex_displacement;
layout(binding = 1) uniform sampler2D m_Texture;
                                                                             
uniform float m_fTime;
in TES_OUT                                                                 
{                                                                          
    vec2 tc;                                                               
    vec3 world_coord;                                                      
    vec3 eye_coord;                                                        
} fs_in;                                                                  
void main(void)                                                            
{                                          
	/*float fTime = (sin(m_fTime / 1000) + 1) * 0.5;
	float fDeltaX = fs_in.tc.x + fTime;
	if (fDeltaX > 1) {
		fDeltaX = 2- fDeltaX;
	}
	vec2 v2TextureCoord = vec2(fDeltaX, fs_in.tc.y);
	vec4 v4Color_1 = texture(tex_color, v2TextureCoord);

	float fDeltaY = fs_in.tc.y + fTime;
	if (fDeltaY > 1) {
		fDeltaY = 2 - fDeltaY;
	}
	v2TextureCoord = vec2(fs_in.tc.x, fDeltaY);
	vec4 v4Color_2 = texture(tex_color, v2TextureCoord);

	FragColor = (v4Color_1 + v4Color_2) / 2;*/
	/*vec3 v3BaseColor = vec3(0, 0.23921568627, 0.47450980392);
	float fNoiseHeight = texture(m_Texture, fs_in.tc).r;
	float fHitPointHeight = texture(tex_displacement, fs_in.tc).r;
	float fTotalHeight = fNoiseHeight + fHitPointHeight;
	vec3 v3Color = vec3(fTotalHeight, fTotalHeight, fTotalHeight) + vec3(0.0, 0.3, 0.5);
	FragColor = vec4(v3BaseColor + vec3(1,1,1) * fHitPointHeight, 1);*/
	//FragColor = vec4(1, 1, 1, 1);
	FragColor = texture(m_Texture, fs_in.tc);
}                                                                          
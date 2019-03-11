#version 420 core                                                              
                                                                               
layout(quads, fractional_odd_spacing) in;                                      
                                                                               
layout(binding = 0) uniform sampler2D tex_displacement;
layout(binding = 1) uniform sampler2D m_Texture;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat4 shadow_matrix;
float dmap_depth = 2.f;                                                      
uniform float Texture_Dmap = 0.5;
uniform float m_fTime;
uniform float m_fTiling = 6;
uniform vec2 m_v2HeightMapPixelSize;
in TCS_OUT { vec2 tc;} tes_in[];                                               
                                                                               
out TES_OUT                                                                    
{                                                                              
    vec2 tc;          
	vec3 m_v3LocalCoord;
    vec3 world_coord;                              
    vec3 eye_coord;            
	vec4 m_v4ClipCoord;
	vec2 m_v2DUDVTextureCoord;
	vec4 m_v4HeightMapNormal;
} tes_out;                                                                     
                          
vec4 CalculateNormal(vec2 _v2TextureCoord) {
	float fBlendValue = texture(tex_displacement, _v2TextureCoord).r;
	float fHeightL = texture(tex_displacement, _v2TextureCoord - (m_v2HeightMapPixelSize.x, 0)).r;
	float fHeightR = texture(tex_displacement, _v2TextureCoord + (m_v2HeightMapPixelSize.x, 0)).r;
	float fHeightD = texture(tex_displacement, _v2TextureCoord - (0, m_v2HeightMapPixelSize.y)).r;
	float fHeightU = texture(tex_displacement, _v2TextureCoord + (0, m_v2HeightMapPixelSize.y)).r;
	vec3 v3ReturnVector = normalize(vec3(fHeightL - fHeightR, 2.f, fHeightD - fHeightU));
	return vec4(v3ReturnVector, fBlendValue);

}
void main(void)                                                                
{                             
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);                
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);                
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);                                   
                                                                               
    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x); 
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x); 
    vec4 p = mix(p2, p1, gl_TessCoord.y);                                      
    p.y += texture(tex_displacement, tc).r * dmap_depth;
	//p.y -= texture(m_Texture, tc).r * Texture_Dmap;
	/*float fTime = (sin(m_fTime / 1000) + 1) * 0.5;
	float fDeltaX = tc.x + fTime;
	if (fDeltaX > 1) {
		fDeltaX = 2 - fDeltaX;
	}
	vec2 v2TextureCoord = vec2(fDeltaX, tc.y);


	p.y -= texture(m_Texture, v2TextureCoord).r * Texture_Dmap;

	float fDeltaY = tc.y + fTime;
	if (fDeltaY > 1) {
		fDeltaY = 2 - fDeltaY;
	}
	v2TextureCoord = vec2(tc.x, fDeltaY);


	p.y -= texture(m_Texture, v2TextureCoord).r * Texture_Dmap;*/
    vec4 P_eye = view_matrix * model_matrix * p;                                                
                                                                               
    tes_out.tc = tc * 1;                                                           
    tes_out.world_coord = (model_matrix * p).xyz;
    tes_out.eye_coord = P_eye.xyz;                                             
                                        
	tes_out.m_v4ClipCoord = proj_matrix * P_eye;
	tes_out.m_v2DUDVTextureCoord = vec2(tes_out.m_v4ClipCoord.x * 0.5 + 0.5, tes_out.m_v4ClipCoord.y * 0.5 + 0.5) * m_fTiling;
	tes_out.m_v4HeightMapNormal = CalculateNormal(tes_out.tc);
	tes_out.m_v3LocalCoord = p.xyz;
    gl_Position = tes_out.m_v4ClipCoord;
}                                                                              
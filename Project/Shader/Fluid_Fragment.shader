#version 420 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D tex_displacement;
layout(binding = 1) uniform sampler2D m_Texture;
layout(binding = 2) uniform sampler2D m_ReflectTexture;
layout(binding = 3) uniform sampler2D m_RefractTexture;
layout(binding = 4) uniform sampler2D m_DUDVTexture;
layout(binding = 5) uniform sampler2D m_NormalMapTexture;
layout(binding = 6) uniform sampler2D m_DepthTexture;
layout(binding = 7) uniform sampler2D shadow_tex;

uniform float m_fTime;
uniform float m_fMoveFactor;
uniform vec4 m_v4Plane = vec4(0, 1, 0, 1);
uniform vec3 m_v3EyePosition;
uniform vec3 m_v3ViewingDirection;
uniform vec3 m_v3LightPosition = vec3(1000, 1000, 1000);
uniform mat4 model_matrix;
uniform mat4 shadow_matrix;
float m_fSpecularValue = 200;
vec3 m_v3SpecularColor = vec3(1, 1, 1);
float m_fReflectivity = 0.2f;
float m_fDistortStrengh = 0.04;
float m_fNearPlane = 0.1f;
float m_fFarPlane = 100000;
float m_fAmbientValue = 1;
float m_fTiling = 20;
float m_fNormalSmoothValue = 3;
in TES_OUT
{
	vec2 tc;
vec3 m_v3LocalCoord;
vec3 world_coord;
vec3 eye_coord;
vec4 m_v4ClipCoord;
vec2 m_v2DUDVTextureCoord;
vec4 m_v4HeightMapNormal;
} fs_in;
float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2DistortCoord_1 = texture(m_DUDVTexture, fs_in.tc + vec2(m_fMoveFactor, m_fMoveFactor)).xy*0.1;
	vec2 v2DistortCoord_2 = (fs_in.tc + vec2(v2DistortCoord_1.x, v2DistortCoord_1.y + m_fMoveFactor)) * 1;
	vec2 v2DistortCoord = (texture(m_DUDVTexture, v2DistortCoord_2).xy * 2 - 1) * 1;

	vec4 v4DistortLocalPosition = vec4(fs_in.m_v3LocalCoord.x + v2DistortCoord.x, fs_in.m_v3LocalCoord.y, fs_in.m_v3LocalCoord.z + v2DistortCoord.y, 1);
	vec4 v4ShadowCoord = shadow_matrix * model_matrix * v4DistortLocalPosition;

	vec2 v2Coord = v4ShadowCoord.xy / v4ShadowCoord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(shadow_tex, v2Coord).r < (v4ShadowCoord.z / v4ShadowCoord.w)) {
			ReturnValue = 0.3f;
		}
	}
	return ReturnValue;
}
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
	vec2 v2DistortCoord_1 = texture(m_DUDVTexture, fs_in.tc + vec2(m_fMoveFactor, m_fMoveFactor)).xy*0.1;
	vec2 v2DistortCoord_2 = (fs_in.tc + vec2(v2DistortCoord_1.x, v2DistortCoord_1.y + m_fMoveFactor)) * m_fTiling;
	vec2 v2DistortCoord = (texture(m_DUDVTexture, v2DistortCoord_2).xy * 2 - 1) * m_fDistortStrengh;
	vec2 v2ReflectCoord = (fs_in.m_v4ClipCoord.xy * vec2(1, -1) / fs_in.m_v4ClipCoord.w) * 0.5 + 0.5;
	vec2 v2RefractCoord = (fs_in.m_v4ClipCoord.xy / fs_in.m_v4ClipCoord.w) * 0.5 + 0.5;

	v2ReflectCoord = clamp(v2ReflectCoord, 0.001, 0.999);
	v2RefractCoord = clamp(v2RefractCoord, 0.001, 0.999);


	vec4 v4ReflectColor = texture(m_ReflectTexture, v2ReflectCoord + v2DistortCoord);
	//vec4 v4ReflectColor = texture(m_ReflectTexture, v2ReflectCoord);
	//vec4 v4RefractColor = texture(m_RefractTexture, v2RefractCoord + v2DistortCoord);
	vec4 v4RefractColor = texture(m_RefractTexture, v2RefractCoord + v2DistortCoord);
	//FragColor = v4RefractColor;

	vec3 v3ViewingDirection = normalize(fs_in.world_coord - m_v3EyePosition);

	float fFresnelValue = pow(dot(-m_v3ViewingDirection, m_v4Plane.xyz), 2);


	float fDepthValue = texture(m_DepthTexture, v2RefractCoord).r;
	float fFloorDepth = 2 * m_fNearPlane*m_fFarPlane / (m_fFarPlane + m_fNearPlane - (2 * fDepthValue - 1) * (m_fFarPlane - m_fNearPlane));
	fDepthValue = gl_FragCoord.z;
	float fWaterDepth = 2 * m_fNearPlane*m_fFarPlane / (m_fFarPlane + m_fNearPlane - (2 * fDepthValue - 1) * (m_fFarPlane - m_fNearPlane));

	float fWaterDistance = fFloorDepth - fWaterDepth;
	fWaterDistance = clamp(fWaterDistance / 1, 0.0, 1.0);

	vec3 v3BaseColor = mix(mix(v4ReflectColor, v4RefractColor, fFresnelValue), vec4(0, 0.3, 0.5, 1), 0.2).xyz;
	//FragColor = texture(m_NormalMapTexture, v2DistortCoord_2);
	//FragColor = vec4(v3BaseColor,1);

	//Calculate Normal
	vec4 v4NormalColor = texture(m_NormalMapTexture, v2DistortCoord_2);
	vec3 v3NormalVector = normalize(vec3(v4NormalColor.x * 2 - 1, v4NormalColor.z * m_fNormalSmoothValue, v4NormalColor.y * 2 - 1));
	v3NormalVector = mix(v3NormalVector, fs_in.m_v4HeightMapNormal.xyz, fs_in.m_v4HeightMapNormal.w);
	vec3 v3LightVector = normalize(fs_in.world_coord - m_v3LightPosition);
	vec3 v3RelectedVector = reflect(v3LightVector, v3NormalVector);

	float fHitPointHeight = texture(tex_displacement, fs_in.tc).r;
	float fSpecular = pow(max(dot(v3RelectedVector, -v3ViewingDirection), 0), m_fSpecularValue) + fHitPointHeight * 2;
	vec3 v3SpecularColor = m_v3SpecularColor * fSpecular * m_fReflectivity;

	vec3 v3DiffuseColor = max(dot(-v3LightVector, v3NormalVector), 0) * v3BaseColor * (1 - m_fAmbientValue);

	//vec3 v3BlingPhoneColor = v3SpecularColor + v3DiffuseColor + v3BaseColor * m_fAmbientValue;
	vec3 v3BlingPhoneColor = v3SpecularColor + v3BaseColor;
	//vec3 v3BlingPhoneColor =  v3BaseColor;
	//FragColor = texture(m_DUDVTexture, fs_in.tc);
	//FragColor = mix(v4ReflectColor, v4RefractColor, fFresnelValue);
	FragColor = vec4(v3BlingPhoneColor, fWaterDistance);
	//FragColor = vec4(fWaterDistance, fWaterDistance, fWaterDistance, 1);
	//FragColor.a = fWaterDistance;
	//FragColor = v4RefractColor;
	//FragColor = mix(v4ReflectColor, v4RefractColor, 1);
	//FragColor = v4RefractColor;
	//FragColor = vec4(1, 1, 1, 1);
}
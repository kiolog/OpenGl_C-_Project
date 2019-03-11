#version 420 core
layout(binding = 0) uniform sampler2D m_Texture;
layout(binding = 1) uniform sampler2D m_ShadowMap;
out vec4 FragColor;

vec3 m_v3SpecularColor = vec3(1, 1, 1);
float m_fSpecular_Power = 10.f;
float m_fDiffuseValue = 0.7;
in VS_OUT
{
	vec2 m_v2TextureCoord;
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 H;
} fs_in;
float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2Coord = fs_in.shadow_coord.xy / fs_in.shadow_coord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(m_ShadowMap, fs_in.shadow_coord.xy / fs_in.shadow_coord.w).r < (fs_in.shadow_coord.z / fs_in.shadow_coord.w)) {
			ReturnValue = 0.0f;
		}
	}
	return ReturnValue;
}
void main(void)
{

	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 H = normalize(fs_in.H);

	vec3 v3BaseColor = texture(m_Texture, fs_in.m_v2TextureCoord).rgb;

	vec3 v3Diffuse = max(dot(N, L), 0.0) * v3BaseColor * m_fDiffuseValue;
	vec3 v3Ambient = v3BaseColor * (1 - m_fDiffuseValue);
	vec3 v3Specular = pow(max(dot(N, H), 0.0), m_fSpecular_Power) * m_v3SpecularColor;
	FragColor = vec4((v3Diffuse) + v3Ambient, 1);
	//FragColor = vec4((v3Diffuse + v3Specular) + v3Ambient, 1);
	//FragColor = texture(m_Texture, fs_in.m_v2TextureCoord);
}
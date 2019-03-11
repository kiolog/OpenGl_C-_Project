#version 420 core

layout(binding = 0) uniform samplerCube m_CubeMap;
layout(binding = 1) uniform sampler2D shadow_tex;
out vec4 FragColor;

in VS_OUT
{
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 V;
} fs_in;


uniform int m_iDrawWithShadow;
float fSpecular_Power = 200.f;
vec3 m_v3SpecularColor = vec3(0.7, 0.7, 0.7);
vec3 m_v3DiffuseColor = vec3(0.35, 0.35, 0.35);

float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2Coord = fs_in.shadow_coord.xy / fs_in.shadow_coord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(shadow_tex, fs_in.shadow_coord.xy / fs_in.shadow_coord.w).r < (fs_in.shadow_coord.z / fs_in.shadow_coord.w)) {
			ReturnValue = 0.2f;
		}
	}
	return ReturnValue;
}
void main(void)
{
	// Normalize the incoming N, L and V vectors
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	vec3 H = normalize(L + V);
	// Compute the diffuse and specular components for each fragment
	vec3 r = reflect(-fs_in.V, normalize(fs_in.N));
	vec3 diffuse = max(dot(N, L), 0.0) * m_v3DiffuseColor;
	vec3 specular = pow(max(dot(N, H), 0.0), fSpecular_Power) * m_v3SpecularColor;
	vec3 EnviromentColor = texture(m_CubeMap, r).xyz;
	vec3 v3FinalColor = (diffuse + specular) * 0.65 + EnviromentColor * 0.35;
	v3FinalColor = v3FinalColor * CalculateShadowValue();
	//FragColor = vec4((diffuse + specular) * 0.65 + EnviromentColor * 0.35, 1);
	FragColor = vec4(v3FinalColor, 1);
}
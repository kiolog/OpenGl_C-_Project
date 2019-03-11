#version 420 core
layout(binding = 0) uniform sampler2D tex;
layout(binding = 1) uniform sampler2D shadow_tex;

out vec4 FragColor;

in VS_OUT
{
	vec2 TextureCoord;
	vec4 shadow_coord;
	vec3 N;
	vec3 L;
	vec3 V;
} fs_in;

vec3 v3SpecularColor = vec3(1, 1, 1);
float fSpecular_Power = 10.f;
float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2Coord = fs_in.shadow_coord.xy / fs_in.shadow_coord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(shadow_tex, fs_in.shadow_coord.xy / fs_in.shadow_coord.w).r < (fs_in.shadow_coord.z / fs_in.shadow_coord.w)) {
			ReturnValue = 0.0f;
		}
	}
	return ReturnValue;
}
void main(void)
{
	float fDiffuseValue = 0.9f;
	vec3 v3DiffuseColor = texture(tex, fs_in.TextureCoord).rgb;
	// Normalize the incoming N, L and V vectors
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	vec3 H = normalize(L + V);
	// Compute the diffuse and specular components for each fragment
	vec3 diffuse = max(dot(N, L), 0.0) * v3DiffuseColor * fDiffuseValue;
	vec3 ambient = v3DiffuseColor * (1 - fDiffuseValue);
	vec3 specular = pow(max(dot(N, H), 0.0), fSpecular_Power) * v3SpecularColor;
	//color = textureProj(shadow_tex, fs_in.shadow_coord) *vec4(v3DiffuseColor, 1.0);
	//FragColor = vec4(vec3(CalculateShadowValue()), 1);
	FragColor = vec4((CalculateShadowValue() * (diffuse + specular)) + ambient,1);
	//FragColor = fs_in.shadow_coord;
	//FragColor = vec4(1, 1, 1, 1);
	//color = texture(tex, fs_in.TextureCoord);
	//color = vec4(1, 0, 0, 1);
	//color = vec4(v3DiffuseColor, 1);
	FragColor = vec4(1,1,1, 1);
}
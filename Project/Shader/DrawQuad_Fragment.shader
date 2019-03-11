#version 420 core
layout(binding = 1) uniform sampler2D shadow_tex;

uniform int m_iDrawWithShadow;
out vec4 FragColor;
in VS_OUT
{
	vec4 shadow_coord;
} fs_in;

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
	if (m_iDrawWithShadow == 1) {
		float fShadowValue = CalculateShadowValue();
		if (fShadowValue <= 0.5f) {
			FragColor = vec4(0.41, 0.36, 0.37, 1);
		}
		else {
			FragColor = vec4(0.64, 0.57, 0.49, 1);
		}
	}
	else {
		FragColor = vec4(0.64, 0.57, 0.49, 1);
	}
	
}
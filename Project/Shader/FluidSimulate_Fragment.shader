#version 420
layout(binding = 0) uniform sampler2D LastMap_1;
layout(binding = 1) uniform sampler2D LastMap_2;

uniform float m_fTravelSpeed = 0.1;
uniform float m_fDampening = 0.999f;
uniform float m_fTexelSize = 0.003906;
out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
vec2[4] OffsetArray = vec2[4](vec2(-1, 0), vec2(1, 0), vec2(0, -1), vec2(0, 1));
void main(void)
{
	vec3 v3ShiftedColor = vec3(0,0,0);
	for (int i = 0; i < 4; ++i) {
		vec2 v2Offset = OffsetArray[i] * m_fTexelSize;
		vec2 v2TextureCoord = fs_in.texcoord + v2Offset;
		v3ShiftedColor += texture(LastMap_1, v2TextureCoord).xyz;
	}
	vec3 v3FourTimesLast1 = (texture(LastMap_1, fs_in.texcoord).xyz * 4);
	//v3ShiftedColor -= v3FourTimesLast1;
	v3ShiftedColor = ((v3ShiftedColor - v3FourTimesLast1) * m_fTravelSpeed) + v3FourTimesLast1;
	v3ShiftedColor = (v3ShiftedColor * 0.5 - texture(LastMap_2, fs_in.texcoord).xyz) * m_fDampening;
	FragColor = vec4(v3ShiftedColor, 1);
}
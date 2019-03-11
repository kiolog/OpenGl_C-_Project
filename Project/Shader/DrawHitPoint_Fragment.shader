#version 420
layout(binding = 0) uniform sampler2D Point_Texture;
layout(binding = 1) uniform sampler2D FluidMap;

uniform vec2 m_v2HitPosition;
uniform float m_fOutSidePointScale = 0.02f;
uniform float m_fInSidePointScale = 0.015f;
uniform int m_bHit;
out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
void main(void)
{
	if (m_bHit == 1) {
		vec3 v3RenderTargetColor = texture(FluidMap, fs_in.texcoord).xyz;
		float fDistance = 0.5 * m_fOutSidePointScale;
		vec2 RelativePosition = fs_in.texcoord - m_v2HitPosition;
		if (abs(RelativePosition.x) <= fDistance && abs(RelativePosition.y) <= fDistance) {
			vec2 v2PointTextureCoord = (RelativePosition + fDistance) / (2 * fDistance);
			vec3 v3PointColor = texture(Point_Texture, v2PointTextureCoord).xyz;
			//vec3 v3PointColor = vec3(0, 0, 0);
			fDistance = 0.5 * m_fInSidePointScale;
			if (abs(RelativePosition.x) <= fDistance && abs(RelativePosition.y) <= fDistance) {
				v2PointTextureCoord = (RelativePosition + fDistance) / (2 * fDistance);
				v3PointColor -= texture(Point_Texture, v2PointTextureCoord).xyz;
				//v3PointColor *= 1.5;
			}
			FragColor = vec4(v3RenderTargetColor + v3PointColor, 1);
		}
		else {
			FragColor = vec4(v3RenderTargetColor, 1);
		}
		//FragColor = vec4(texture(Point_Texture, fs_in.texcoord).xyz + texture(FluidMap, fs_in.texcoord).xyz, 1);
	}
	else {
		FragColor = texture(FluidMap, fs_in.texcoord);
	}
	
}
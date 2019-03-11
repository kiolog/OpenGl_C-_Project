#version 420
uniform float m_fTime;
out vec4 FragColor;
in VS_OUT
{
	vec2 texcoord;
} fs_in;
int m_iMaxIteration = 8;
void main(void)
{
	float fTime = m_fTime / 100;
	//vec2 p = fs_in.texcoord * m_v2ImageSize * 8.0 - vec2(20.0);
	vec2 p = fs_in.texcoord * 8.0 - vec2(20.0);
	vec2 i = p;
	float c = 1.0;
	float inten = .02;

	for (int n = 0; n < m_iMaxIteration; n++)
	{
		float t = fTime + c * c / 40. - p.x*0.2;
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0 / length(vec2(p.x / (sin(i.x + t) / inten), p.y / (cos(i.y + t) / inten)));
	}
	c /= float(m_iMaxIteration);
	c = 1.5 - sqrt(c);
	
	c = pow(c, 2);
	vec3 v3WaterColor = vec3(c*c*c*c) + vec3(0.0, 0.3, 0.5);
	FragColor = vec4(c,c,c, 1);
	//vec2 v2TextureCoord = fs_in.texcoord + 0.05f * vec2(c - 0.5);
	
}
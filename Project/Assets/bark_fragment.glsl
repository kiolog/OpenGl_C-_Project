#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4m;
uniform mat4 um4v;
uniform mat4 um4p;
in vec3 vv3normal;
in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;

uniform sampler2D tex;

uniform vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 200.0;
uniform vec3 ambient = vec3(0.225, 0.2, 0.25); 
void main()
{
    
	//vec4 color = texture(tex, vertexData.texcoord) ;
	vec4 color= vec4(0.545f, 0.270f, 0.185, 1);
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 V = normalize(vertexData.H);
	//vec3 H = normalize(L+V);
	vec3 H = (L+V) / sqrt( (L.x+V.x)*(L.x+V.x) + (L.y+V.y)*(L.y+V.y) + (L.z+V.z)*(L.z+V.z) );

	vec4 ka = color;
	vec4 kd = color;
	//vec3 kd = color.rgb;

	/*if(vv3normal.y > 0.3)
		{
			ka = vec4(1.0);
			kd = vec4(1.0);
		}*/
	vec4 ks = vec4(1, 1, 1,1.0);
	vec4 ia = vec4(0.3, 0.3, 0.3,1.0);
	vec4 id = vec4(0.5, 0.5, 0.5,1.0);
	vec4 is = vec4(1, 1, 1,1.0);
	vec4 IP = ka*ia + max(dot(L, N), 0.0) * kd*id  +  ks*is * pow(max(dot(H, N), 0.0), 100);

	fragColor = vec4(IP.rgb, 1.0);
	
}
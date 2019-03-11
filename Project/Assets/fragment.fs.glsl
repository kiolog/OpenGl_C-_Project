#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4v;
uniform mat4 um4m;
uniform mat4 um4p;
uniform vec3 cameraPos;
uniform sampler2D tex;
uniform sampler2D nor;
uniform vec3 lightPos;
uniform int normalAsColor;
uniform int normalMap;

in VertexData
{
	vec3 normal;
	vec3 fragPos;
    vec2 texcoord;
	mat3 TBN;
} vertexData;



void main()
{
	uint shininess = 1; 
	float specularStrength = 0.1f;
	vec3 lightPos_ = vec3(um4p * um4v * um4m * vec4(lightPos,1));
	vec3 normal = normalize(vertexData.normal);
	if (1 == normalMap) 
	{
		normal = (texture(nor, vertexData.texcoord).rgb * 2) - 1;
		normal = normalize(vertexData.TBN * normal);
		shininess = 5;
		specularStrength = 0.8f;
	}
	if (1==normalAsColor)
	{
		fragColor = vec4(abs(normal), 1.0);
	}
	else 
	{
		vec3 lightColor = vec3(1, 1, 1);
		// ambient light
		float ambientStrength = 0.9f;
		vec4 ambient = texture(tex, vertexData.texcoord);
		// diffuse light
		vec3 lightDir = normalize(lightPos_ - vertexData.fragPos);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
		// specular highlight
		
		vec3 viewDir = normalize(cameraPos - vertexData.fragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		vec3 specular = specularStrength * spec * lightColor;

		fragColor = ambient + vec4( 0.1* diffuse + specular,0 );
	}
}
#version 410

in VS_OUT{
	flat vec3 surfaceNormal;
	flat vec4 color;
	flat vec3 toLightVector;
	flat vec3 toCameraVector;
	flat float snow;
} pass ;


out vec4 out_Color;
uniform float winterFraction;

void main(void){
	vec3 diff_tex = mix(pass.color.rgb, vec3(1,1,1), pass.snow);
	vec3 ka = diff_tex;
	vec3 kd = diff_tex;
	vec3 ks = vec3(1);

	vec3 Ia = vec3(0.9);
	vec3 Id = vec3(0.4);
	vec3 Is = mix(vec3(0.1), vec3(0.05), winterFraction);
	float n = 10;
	 
	vec3 N = normalize(pass.surfaceNormal);  
	vec3 L = normalize(pass.toLightVector);  
	vec3 V = normalize(pass.toCameraVector);  
	vec3 H = normalize(L + V); 
	 
	vec3 ambient = ka * Ia;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * Id;  
	vec3 specular = pow(max(dot(N, H), 0.0), n) * ks * Is;
	vec3 BlinnPhongColor = ambient + diffuse + specular;

	out_Color = vec4(BlinnPhongColor.xyz,pass.color.w);
}
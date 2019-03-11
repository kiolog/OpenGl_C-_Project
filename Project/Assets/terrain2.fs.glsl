#version 410
uniform sampler2D m_ShadowMap;
in VS_OUT{
	flat vec3 surfaceNormal;
	flat vec4 color;
	vec3 toLightVector;
	vec3 toCameraVector;
	vec4 c;
	vec4 shadow_coord;
} pass ;


out vec4 out_Color;
float CalculateShadowValue() {
	float ReturnValue = 1.0f;
	vec2 v2Coord = pass.shadow_coord.xy / pass.shadow_coord.w;
	if (v2Coord.x >= 0 && v2Coord.x <= 1 && v2Coord.y >= 0 && v2Coord.y <= 1) {
		if (texture(m_ShadowMap, pass.shadow_coord.xy / pass.shadow_coord.w).r < (pass.shadow_coord.z / pass.shadow_coord.w)) {
			ReturnValue = 0.0f;
		}
	}
	return ReturnValue;
}


void main(void){
	// if(pass.color.w == 0)
	// 	discard;
	// vec4 diff_tex = vec4(0.5,0.25,0.5,1.0);
	vec4 diff_tex = pass.color;
	vec3 ka = diff_tex.rgb;
	vec3 kd = diff_tex.rgb;
	vec3 ks = vec3(1);

	vec3 Ia = vec3(0.9);
	vec3 Id = vec3(0.35);
	vec3 Is = vec3(0.1);
	float n = 10;
	 
	vec3 N = normalize(pass.surfaceNormal);  
	vec3 L = normalize(pass.toLightVector);  
	vec3 V = normalize(pass.toCameraVector);  
	vec3 H = normalize(L + V); 
	 
	vec3 ambient = ka * Ia;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * Id;  
	vec3 specular = pow(max(dot(N, H), 0.0), n) * ks * Is;
	vec4 BlinnPhongColor = vec4(ambient + diffuse + specular, 1.0);

	// if(gl_FragCoord.x < 500)
	//out_Color = vec4(CalculateShadowValue() * BlinnPhongColor.xyz,pass.color.w);
	out_Color = vec4(BlinnPhongColor.xyz, pass.color.w);
	// else
		// out_Color = pass.c;
}
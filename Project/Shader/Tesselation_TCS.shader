#version 410 core                                                             
                                                                              
layout (vertices = 4) out;                                                    
                                                                              
in VS_OUT{ 
	vec2 tc;
} tcs_in[];                                                
out TCS_OUT{ 
	vec2 tc;
} tcs_out[];                                             
                                         
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform vec3 m_v3EyePosition; 

int m_iMaxInnerSize = 16;
float m_fDistanceInterval = 10;
                            
int ComputeOuterSize(vec4 _v4Point1,vec4 _v4Point2) {
	int iReturnValue = 1;

	vec3 v3Point1 = _v4Point1.xyz;
	vec3 v3Point2 = _v4Point2.xyz;

	vec3 v3MiddlePoint = (v3Point1 + v3Point2) * 0.5;
	float fDistance = length(-m_v3EyePosition - v3MiddlePoint);
	int iDivider = int(fDistance / m_fDistanceInterval);
	float fOuterSize = m_iMaxInnerSize / pow(2, iDivider);
	if (fOuterSize > 1) {
		iReturnValue = int(fOuterSize);
	}
	iReturnValue = 1;
	return iReturnValue;
}

void main(void)                                                               
{                                                                             
    if (gl_InvocationID == 0)                                                 
    {                  
		mat4 mvp_matrix = proj_matrix * view_matrix * model_matrix;
        vec4 p0 = mvp_matrix * gl_in[0].gl_Position;                          
        vec4 p1 = mvp_matrix * gl_in[1].gl_Position;                          
        vec4 p2 = mvp_matrix * gl_in[2].gl_Position;                          
        vec4 p3 = mvp_matrix * gl_in[3].gl_Position;                          
        p0 /= p0.w;                                                           
        p1 /= p1.w;                                                           
        p2 /= p2.w;                                                           
        p3 /= p3.w;                                                           
        if (p0.z <= -1.0 &&                                                   
            p1.z <= -1.0 &&                                                   
            p2.z <= -1.0 &&                                                   
            p3.z <= -1.0)                                                     
         {                                                                    
              gl_TessLevelOuter[0] = 0.0;                                     
              gl_TessLevelOuter[1] = 0.0;                                     
              gl_TessLevelOuter[2] = 0.0;                                     
              gl_TessLevelOuter[3] = 0.0;                                     
         }                                                                    
         else                                                                 
         {          
			vec4 p0_World = model_matrix * gl_in[0].gl_Position;
			vec4 p1_World = model_matrix * gl_in[1].gl_Position;
			vec4 p2_World = model_matrix * gl_in[2].gl_Position;
			vec4 p3_World = model_matrix * gl_in[3].gl_Position;
			p0_World /= p0_World.w;
			p1_World /= p1_World.w;
			p2_World /= p2_World.w;
			p3_World /= p3_World.w;
            int l0 = ComputeOuterSize(p2_World, p0_World);
			int l1 = ComputeOuterSize(p3_World, p2_World);
			int l2 = ComputeOuterSize(p3_World, p1_World);
			int l3 = ComputeOuterSize(p1_World, p0_World);
            gl_TessLevelOuter[0] = l0;                                        
            gl_TessLevelOuter[1] = l1;                                        
            gl_TessLevelOuter[2] = l2;                                        
            gl_TessLevelOuter[3] = l3;                                        
            gl_TessLevelInner[0] = min(l1, l3);                               
            gl_TessLevelInner[1] = min(l0, l2);                               
        }                                                                     
    }                                                                         
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;     

}                                                                             
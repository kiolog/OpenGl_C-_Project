#version 420 core                                                                     
                                                                                      
out VS_OUT{ 
	vec2 tc;
} vs_out;       
layout(binding = 0) uniform sampler2D tex_displacement;
uniform vec2 m_v2TerrainSize;
uniform vec4 m_v4Plane = vec4(0, 1, 0, 1);
uniform float dmap_depth = 6;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
void main(void)                                                                       
{                                                
	//gl_ClipDistance[0] = 10;
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),                        
                                   vec4( 0.5, 0.0, -0.5, 1.0),                        
                                   vec4(-0.5, 0.0,  0.5, 1.0),                        
                                   vec4( 0.5, 0.0,  0.5, 1.0));                       
                                                                                      
    int x = int(mod(gl_InstanceID,m_v2TerrainSize.x));
    int y = int(gl_InstanceID / m_v2TerrainSize.x);
    vec2 offs = vec2(x, y);                                                           
                                                                                      
    vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / m_v2TerrainSize;
	float fHeight = texture(tex_displacement, vs_out.tc).r * dmap_depth;
	gl_Position = vertices[gl_VertexID] + vec4(float(x - m_v2TerrainSize.x / 2), 0.0, float(y - m_v2TerrainSize.y / 2), 0.0);
	//gl_Position = proj_matrix * view_matrix * model_matrix * gl_Position;
	//vec4 v4WorldPosition = model_matrix * gl_Position;
	//gl_ClipDistance[0] = dot(v4WorldPosition, m_v4Plane);
	//gl_ClipDistance[0] = -1;
}                                                                                     
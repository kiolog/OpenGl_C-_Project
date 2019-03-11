#version 410

layout(location = 0) out vec4 fragColor;
in vec2 texcoord;
uniform sampler2D tex;
void main()
{
	//fragColor = vec4(1.0);
    //fragColor = texture(tex, texcoord);
	//if(fragColor.rgb == vec3(0))
		fragColor = vec4(1.0f);

}
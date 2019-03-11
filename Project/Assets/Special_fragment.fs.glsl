#version 430 core

uniform sampler2D tex;
uniform int ShaderChooser;
uniform int Window_W;
uniform int Window_H;
uniform int Mouse_X;
uniform int Mouse_Y;
uniform int SimpleCounter;
uniform int Comparison_Bar;
uniform float RandomNumber[30];

out vec4 color;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

vec4 Default(){
	return texture(tex, fs_in.texcoord);
}

float toGrey(vec3 v)
{
	return dot(v,vec3(0.22,0.587,0.114));
};

vec2 cursorTocenter(){
	vec2 dxdy;
	dxdy[0] = float(Mouse_X) / float(Window_W) - 0.5f;
	dxdy[1] = float(Mouse_Y) / float(Window_H) - 0.5f;
	dxdy *= 2.0f;
	return dxdy;
}

vec4 fastBlur(){
	float kernel[5][5] = {
		{0.031827, 0.037541, 0.039665, 0.037541, 0.031827},
		{0.037541, 0.044281, 0.046787, 0.044281, 0.037541},
		{0.039665, 0.046787, 0.049434, 0.046787, 0.039665},
		{0.037541, 0.044281, 0.046787, 0.044281, 0.037541},
		{0.031827, 0.037541, 0.039665, 0.037541, 0.031827}
		};
	vec4 pixColor = vec4(0);
	float cnt = 0;
	for(int i = -2 ; i <= 2 ; i++){
		for(int j = -2; j <= 2 ; j++){
			vec4 I = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			pixColor += I * kernel[i+2][j+2];
			cnt +=kernel[i+2][j+2];
		}
	}
	pixColor /= cnt;
	return vec4(pixColor.rgb,1);
}

vec4 Red_Blue()
{
	float offset = 0.005;
	vec4 texture_color_Left  = texture(tex, fs_in.texcoord - offset * vec2(1.0, 0.0));
	vec4 texture_color_Right = texture(tex, fs_in.texcoord + offset * vec2(1.0, 0.0));
	vec4 texture_color = vec4(1.0, 1.0, 1.0, 1.0);
	float color_R = texture_color_Left.r * 0.299 + texture_color_Left.g* 0.587 + texture_color_Left.b * 0.114;
	float color_G = texture_color_Right.g;
	float color_B = texture_color_Right.b;
	return vec4(color_R, color_G, color_B,1);
}

vec4 DoG()
{
	float sigmaA = 2.0f;
	float sigmaB = 2.8f;
	vec2 sigma = vec2(sigmaA, sigmaB);
	vec2 sigmaSquare = vec2(sigmaA*sigmaA, sigmaB*sigmaB);
	vec2 norm = (2 * 3.14 * sigmaSquare);
	int halfKernalSize = 5;
	vec2 dog;
	for(int i = -halfKernalSize ; i <= halfKernalSize ; i++){
		for(int j = -halfKernalSize; j <= halfKernalSize ; j++){
			vec4 I_tex = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			vec2 I_gray = vec2(toGrey(I_tex.rgb));
			vec2 idx = vec2(i*i+j*j);
			dog += I_gray * exp(-idx/sigmaSquare);
		}
	} 
	dog /= norm;

	float frac = 0.99f;
	float H = 100.0 * (dog.x - frac * dog.y);
	float edge = ( H > 0.0 ) ? 1.0 : 2.0 * smoothstep( -2.0, 2.0, 3.4 * H);
	vec4 pixColor = vec4(vec3(edge),1);
	
	return pixColor;
}

vec4 LoG_()
{
	float sigma = 0.5;
	int halfKernalSize = 4;
	float sigma2 = sigma* sigma;
	float coef = -(3.1415926 * sigma2 * sigma2);
	vec4 pixColor;
	for(int i = -halfKernalSize ; i <= halfKernalSize ; i++){
		for(int j = -halfKernalSize; j <= halfKernalSize ; j++){
			float p = -1 * (i*i + j*j) / (2 * sigma2);
			vec4 I = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			I = vec4(vec3(toGrey(I.rgb)), 1);
			float kernel = (1 + p) * exp(p);
			pixColor += I * kernel;		
		}
	} 
	pixColor /= coef;
	return vec4(pixColor.rgb,1);
}

vec4 Laplacian()
{
	mat3 kernel;
	kernel[0] = vec3(-1,-1,-1);
	kernel[1] = vec3(-1, 8,-1);
	kernel[2] = vec3(-1,-1,-1);
	int halfKernalSize = 1;
	vec4 pixColor;
	for(int i = -halfKernalSize ; i <= halfKernalSize ; i++){
		for(int j = -halfKernalSize; j <= halfKernalSize ; j++){
			vec4 I = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			I = vec4(vec3(toGrey(I.rgb)), 1);
			pixColor += I * kernel[i+1][j+1];
		}
	}
	if (pixColor.r > 0.1)
		return vec4(1);
	else
		return vec4(vec3(0),1); 
}

vec4 Sharpen(){
	mat3 kernel;
	kernel[0] = vec3( 0,-1, 0);
	kernel[1] = vec3(-1, 5,-1);
	kernel[2] = vec3( 0,-1, 0);
	int halfKernalSize = 1;
	vec4 pixColor;
	for(int i = -halfKernalSize ; i <= halfKernalSize ; i++){
		for(int j = -halfKernalSize; j <= halfKernalSize ; j++){
			vec4 I = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			pixColor += I * kernel[i+1][j+1];
		}
	}
	return vec4(pixColor.rgb,1);
}

vec4 Pixelization(){
	float X = gl_FragCoord.x;
	float Y = gl_FragCoord.y;
	float gridSize = 1 + floor(20 * Mouse_Y/Window_H);
	int x = int(gridSize * floor(X/gridSize));
	int y = int(gridSize * floor(Y/gridSize));
	vec4 pixColor;
	for(int i = x ; i < x + gridSize  ; i++){
		for(int j = y; j < y + gridSize ; j++){
			vec4 I = texture(tex, vec2(i,j)/vec2(Window_W,Window_H));
			pixColor += I;
		}
	}
	pixColor /= gridSize*gridSize;
	return vec4(pixColor.rgb,1);
}

vec4 Magnifier(){
	float r = 100;
	float amount = 2;
	vec2 uv = gl_FragCoord.xy - vec2(Mouse_X,Mouse_Y);
	if( length(uv) <= r ){
		uv /= amount;
		return texture(tex, (vec2(Mouse_X,Mouse_Y)+uv)/vec2(Window_W,Window_H));
	}else if(length(uv) <= r + 5)
		return 1 - Default();
	else
		return Default();
}

vec4 Blur(){
	float sigma = 2 + floor(5*abs(cursorTocenter().x));
	int halfKernalSize = int(floor(int(sigma) * 2.5));
	float denominator = 2 * sigma * sigma;
	float norm = 3.14159 * denominator; 
	vec4 pixColor;
	for(int i = -halfKernalSize ; i <= halfKernalSize ; i++){
		for(int j = -halfKernalSize; j <= halfKernalSize ; j++){
			vec4 I = texture(tex, fs_in.texcoord + vec2(i,j)/vec2(Window_W,Window_H));
			pixColor += I * exp(-(i*i+j*j)/denominator);
		}
	}
	pixColor /= norm;
	return vec4(pixColor.rgb,1);
}

vec4 Quantization(){
	float nbins = 2.0 + floor(int(10 * abs(cursorTocenter().y)));
	vec3 tex_color = texelFetch(tex, ivec2(gl_FragCoord.xy), 0).rgb;
	tex_color = floor(tex_color * nbins) / nbins;
	return vec4(tex_color, 1.0);
}

vec4 Image_Abstraction(){
	/// Bit Reduction
	float nbins = 2.0 + floor(int(10 * abs(cursorTocenter().y)));
	vec3 tex_color = Blur().rgb;
	tex_color = floor(tex_color * nbins) / nbins;
	vec4 fin_color = vec4(tex_color, 1.0);
	vec4 edge = DoG();
	return mix(vec4(0,0,0,1), fin_color, edge);
}

vec4 Halftone(){
	float kernel[64] = float[](
	0.0f , 48.0f, 12.0f, 60.0f,  3.0f, 51.0f, 15.0f, 63.0f ,
	32.0f, 16.0f, 44.0f, 28.0f, 35.0f, 19.0f, 47.0f, 31.0f ,
	8.0f , 56.0f,  4.0f, 52.0f, 11.0f, 59.0f,  7.0f, 55.0f ,
	40.0f, 24.0f, 36.0f, 20.0f, 43.0f, 27.0f, 39.0f, 23.0f ,
	2.0f , 50.0f, 14.0f, 62.0f,  1.0f, 49.0f, 13.0f, 61.0f ,
	34.0f, 18.0f, 46.0f, 30.0f, 33.0f, 17.0f, 45.0f, 29.0f ,
	10.0f, 58.0f,  6.0f, 54.0f,  9.0f, 57.0f,  5.0f, 53.0f ,
	42.0f, 26.0f, 38.0f, 22.0f, 41.0f, 25.0f, 37.0f, 21.0f 
	);

	float jubun = toGrey(texture(tex, fs_in.texcoord).rgb);
	vec2 idx = mod(gl_FragCoord.xy,8);
	vec4 finalColor;
	if (jubun >= kernel[int(idx.x)*8+int(idx.y)]/64)
		return vec4(1,1,1,1);
	else
		return vec4(0,0,0,1);
}



vec4 Dots(){
	// Reference : http://weber.itn.liu.se/~stegu/webglshadertutorial/shadertutorial.html
	float gS = 0.5 + floor(2 * abs(cursorTocenter().y));
	float frequency = max(Window_W,Window_H) / gS;
	vec3 white = vec3(1,1,1);
	vec3 black = vec3(0,0,0);
	vec3 blurColor = Blur().rgb; // Mask border smoother
	float r = (1-toGrey(blurColor));
	vec2 rot45 = mat2(0.707, -0.707, 0.707, 0.707) * fs_in.texcoord;
	vec2 nearest = 2.0*fract(frequency * rot45) - 1.0;
	float dist = length(nearest);
	vec3 dotColor = vec3(nearest.x,RandomNumber[int(mod((Mouse_X*Mouse_Y)<<17,30))],nearest.y);
	vec3 fragcolor = mix(dotColor, white, smoothstep(0.8*r,r,length(nearest))); 
	return mix(vec4(fragcolor,1),1 - Laplacian(),vec4(0.2));
}

vec4 Weird(){
	int N = 30;
	vec4 A = vec4(0,1 - Default().g,0,1);
	vec4 B = vec4(0, Halftone().g,0,1);
	vec4 C = Sharpen();
	A = mix(A,vec4(0.5,0,0.5,1),C);
	vec2 M = vec2(Mouse_X,Mouse_Y);
	float blockWidth = 8;
	vec2  blockNum = floor(gl_FragCoord.xy/blockWidth);
	vec2  totblockNum = floor(vec2(Window_W,Window_H)/blockWidth);
	float fallYf = mod(SimpleCounter,totblockNum.y);
	float fallXf[30];
	float dist[30];
	if (distance(gl_FragCoord.xy,M) < 50)
		return vec4(mix(A,B,vec4(0.8)).rgb,1);
	if (distance(gl_FragCoord.xy,M) < 52)
		return 1 - vec4(mix(A,B,vec4(0.8)).rgb,1);


	// if(int(totblockNum.y - blockNum.y) == int(fallYf))
		// return vec4(1,1,1,1);
	
	for(int i = 1; i < N; i++){
		fallXf[i] = floor(RandomNumber[i]*Window_W/blockWidth);
		dist[i] = floor(mod(int(1000*RandomNumber[i])<<11,17)) + 3;
	}
	float toBottom = (totblockNum.y - (fallYf)) / totblockNum.y;

	for(int i = 0; i < N; i++)
		if(blockNum.x == fallXf[i])
			for(float j = 1.0f ; j >= 0.1f ; j -= 0.1)
				if(int(totblockNum.y - blockNum.y) == int(fallYf + toBottom *(2.1*10*j - dist[i])))
					if(i < 15)	
						return vec4(mix(A,B,vec4(j)).rgb,1);
					else
						return 1 - vec4(mix(A,B,vec4(j)).rgb,1);

	return vec4(mix(A,B,vec4(0.22)).rgb,1);
}


void main()
{	

	switch (ShaderChooser){
		case 1:
			color = Red_Blue();			 break;
		case 2:
			color = LoG_();				 break;
		case 3:
			color = Laplacian();		 break;
		case 4:
			color = Sharpen();			 break;
		case 5:
			color = Pixelization(); 	 break;
		case 6:
			color = Magnifier();    	 break;
		case 7:
			color = DoG();				 break;
		case 8:
			color = Blur();				 break;
		case 9:
			color = Quantization(); 	 break;
		case 10:
			color = Image_Abstraction(); break;
		case 11:
			color = Halftone();		 	 break;
		case 12:
			color = Dots();				 break;
		case 13:
			color = Weird();			 break;
		default:
			color = Default();		     break;
	}

	if (Comparison_Bar == 1){
		float BarWidth = 3;
		if(abs(gl_FragCoord.x - Mouse_X) < BarWidth)
			color = vec4(1,0,0,0.5);
		else if(gl_FragCoord.x >= Mouse_X + BarWidth)
			color = Default();
	}
}
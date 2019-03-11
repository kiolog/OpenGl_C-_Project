#ifndef _UTILITIES_
#define _UTILITIES_ 

#include "../Externals/Include/Include.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <cfloat>


using namespace std;
using namespace glm;



class Camera;
class Program;
class Image;
class Mouse;
class SmoothFloat;
class SmoothVec3;


class SimpleTimer{
public:
	SimpleTimer();
	~SimpleTimer();
	
};

class Keyboard
{
	const int DOUBLE_PUSH = 250;
	int cnt = 0;
	int startPoint = 0;
	bool counting = false;
	bool initialized = false;
	unsigned char prev = 0;
	unsigned char curr;
	vec2 currPos;
public:
	Keyboard(){};
	~Keyboard(){};
	void receiveClick(unsigned char newKey, int x, int y){
		if(initialized)
			prev = curr;
		else
			initialized = true;
		curr = newKey;
		currPos = vec2(x,y);
		counting = true;
		cnt = 0;
	}

	bool isCounting(void){
		return counting;
	}
	bool isDoubleClick(void){
		if (initialized && counting && curr == prev && cnt <= DOUBLE_PUSH){
			counting = false;
			cnt = 0;
			return true;
		}
		return false;
	}
	void updateCounter(void){
		if(counting)
			if (++cnt > DOUBLE_PUSH){
				printf(" Keybaord : Time Out\n");
				counting = false;
				cnt = 0;
			}
	}
	unsigned char getCurrKey(void){
		return curr;
	}
	unsigned char getPrevKey(void){
		return prev;
	}
	vec2 getCurrPos(void){
		return currPos;
	}
};

class SmoothFloat {
	
	float agility;
	float target;
	float actual;
	
public:
	SmoothFloat(float initialValue, float _agility){
		target = initialValue;
		actual = initialValue;
		agility = _agility;
	}
	~SmoothFloat(){};

	void update(float delta){
		float offset = target - actual;
		float change = offset * delta * agility;
		actual += change;
	};
	
	void increaseTarget(float dT){	target += dT;		};
	void setTarget(float _target){	target = _target;	};

	float getVal(){		return actual;		};
	float getTarget(){	return target;		};

};

class SmoothVec3 {
	
	float agility;
	vec3 target;
	vec3 actual;
	bool normalization = false;

public:
	SmoothVec3(float x, float y, float z, float _agility = 0.5) {
		actual = target = vec3(x, y, z);
		agility = _agility;
	}
	SmoothVec3(vec3 initialValue, float _agility = 0.5){
		actual = target = initialValue;
		agility = _agility;
	}
	SmoothVec3() {};
	~SmoothVec3(){};

	void update(float delta, bool nor = false){
		vec3 offset = target - actual;
		vec3 change = offset * delta * agility;
		actual += change;
		if (normalization || nor){
			actual = normalize(actual);
		}
	};
	
	void increaseTarget(vec3 dT)	{	target += dT;		};
	void setTarget(vec3 _target)	{	target = _target;	};
	void setNormalization(bool n)	{	normalization = n;  };
	void setVal(vec3 v){ actual = target = v;};
	vec3 getVal(){		return actual;		};
	vec3 getTarget(){	return target;		};
	float getX(void){return actual.x;};
	float getY(void){return actual.y;};
	float getZ(void){return actual.z;};

};

class Mouse
{
	vec2 prev;
	vec2 diff;
	bool valid = false;
public:
	Mouse(){prev = diff = vec2(0);};
	~Mouse(){};
	void MouseFunc(int button, int state, int x, int y){
		prev = vec2(x,y);
		valid = (state == GLUT_DOWN);
	}
	bool MotionFunc(int x, int y){
		vec2 curr = vec2(x,y);
		diff = (valid)? prev - curr : vec2(0);
		prev = curr;
		return valid;
	}

	vec2 getDiff(void){
		return diff;
	}
};

class Image{
	int width 		= 0;
	int height 		= 0;
	int components	= 0;
	int dataSize	= 0;

	float * data = NULL;
	bool loaded = false;
	bool normalized = false;

	void normalize(void){
		float minVal = 300;
		float maxVal = -1;
		for (int i = 0; i < dataSize; ++i){
			minVal = glm::min(minVal, data[i]);	
			maxVal = glm::max(maxVal, data[i]);	
		}
		maxVal -= minVal;
		for (int i = 0; i < dataSize; ++i){
			data[i] -= minVal; data[i] /= maxVal;
		}
		normalized = true;
	}

	template <class T>
	void flipY(T* array){
		for (size_t i = 0; i < width; ++i){
			for (size_t j = 0; j < height / 2 ; ++j){
				size_t coord1 = calcIndex(i,j);
				size_t coord2 = calcIndex(i,height - j - 1);
				swap(array[coord1], array[coord2]);
			}
		}
	}
protected:
	size_t calcIndex(float i, float j,int k = 0, bool fraction = false) {
		if (fraction) {
			if (i < 0 || i > 1 || j < 0 || j > 1) {
				cout << i << endl;
				cout << j;
				cout << "Image : Error when get value in fraction mode\n";
				exit(-1);
			}
			i = (int)(i * width);
			j = (int)(j * height);
		}
		return (j * width + i) * components + k;
	}

	void loadPNG(const char* const pngFilepath, int channel = 4, bool _normalize = false){
		if (data != NULL){
			printf("Clear image in order to load %s\n",pngFilepath);
			free(data);
		}
		unsigned char* data_uc = stbi_load(pngFilepath, &width, &height, &components, channel);
		if (data_uc != NULL){
			components = glm::min(components, channel);
			dataSize = width * height * components;
			data = new float[dataSize];
			flipY(data_uc);
			for (int i = 0; i < dataSize; ++i)
				data[i] = (float) data_uc[i];
			if (_normalize)	normalize();
			free(data_uc);
			loaded = true;
		}else{
			printf("%s Image load failed!!\n", pngFilepath);
			exit(1);
		}
	}
	void isLoaded(void) {
		if (!loaded) {
			printf("Image : Manipulate image before it's loaded.\n");
			exit(-1);
		}
	}

public:
	void load(const char* const imgFilepath, int channel = 4, bool _normalize = false){	loadPNG(imgFilepath, channel, _normalize);	}
	void clearData(void){
		if(data)
			free(data);
		data = NULL;
		loaded = false;
	};
	float getValue(float x, float y, bool fraction = false) {
		isLoaded();
		return data[calcIndex(x, y, fraction)];
	}
	float getValue(float x, float y, float k, bool fraction = false) {
		isLoaded();
		return data[calcIndex(x, y, k, fraction)];
	}
	int getWidth(void) { return width; };
	int getHeight(void) { return height; };
	Image() {};	
	~Image() {	clearData(); };
};



class Camera {
	const float FOV = 80;
	const float NEAR_PLAIN = 0.01f;
	const float FAR_PLAIN = 1000.0f;

	mat4 viewMatrix;
	mat4 projectionMatrix;

protected:
	vec3 lightPos = vec3(50.0f, 10.0f, 50.0f);

	SmoothVec3 position = SmoothVec3(-50.0f,40.0f,-50.0f,0.3);
	SmoothVec3 front = SmoothVec3(1.0f, 0.0f, 0.0f)	;
	vec3 right;
	vec3 top;

	bool flyMode = false;
	bool passiveTurn = true;
	bool useOrthoProjection = false;

	void updateRightAndTop(void){	
		right = normalize(cross(front.getVal(), vec3(0,1,0)));
		top = normalize(cross(right, front.getVal()));
	}
	void updateView(void){
		vec3 target = position.getVal();
		target += front.getVal();
		viewMatrix = lookAt(position.getVal(),  target , top);
	}
public:
	Camera(){};
	~Camera(){};
	void updateProj(vec2& window_size){
		if(!useOrthoProjection)
			projectionMatrix = perspective(
				deg2rad(FOV), 
				window_size.x / window_size.y, 
				NEAR_PLAIN, FAR_PLAIN);
		else
			projectionMatrix = ortho(
				0, (int)window_size.x, (int)window_size.y, 0,
				(int)NEAR_PLAIN, (int)FAR_PLAIN);
	}
	void initialize(vec2& window_size) {
		front.setNormalization(true);
		top = vec3(0,1,0);
		updateRightAndTop();
		updateProj(window_size);
		updateView();
	};

	void turnHeadDrag(vec2 diff){
		if (!passiveTurn) {
			float degX = diff.x * 720;
			float degY = diff.y * 360;
			vec3 curFront = front.getVal();

			float curDegY = degrees(acos(dot(curFront, vec3(0, 1, 0))));
			degY = (degY + curDegY <= 0) ? 10 - curDegY : degY;
			degY = (degY + curDegY >= 180) ? 170 - curDegY : degY;

			mat4 rx = rotate(mat4(), deg2rad(degY), right);
			mat4 ry = rotate(rx, deg2rad(degX), vec3(0, 1, 0));

			front.setTarget(vec3(ry * vec4(curFront, 0.0)));
		}
	}
	void turnHeadPassive(vec2 ax){
		if(passiveTurn){
			float degX = ax.x * 180;
			float degY = ax.y *  90;
			vec3 curFront = front.getVal();

			mat4 rx = rotate(mat4(), deg2rad(degY), right);
			mat4 ry = rotate(rx, deg2rad(degX), vec3(0, 1, 0));

			front.setTarget(vec3(ry * vec4(1,0,0, 0.0)));
		}
	}
	void move(Keyboard& keyboard, float terrainHeight){
		vec3 newPos = position.getVal();
		vec3 frontVal = normalize(vec3(front.getX(),0,front.getZ()));
		vec3 rightVal = normalize(vec3(right.x, 0, right.z));
		switch(keyboard.getCurrKey()){
			case 'w':
			case 'W':
				newPos += 1.5f*frontVal;	break;
			case 'a':
			case 'A':
				newPos -= 1.5f*rightVal;	break;
			case 's':
			case 'S':
				newPos -= 1.5f*frontVal;	break;
			case 'd':
			case 'D':
				newPos += 1.5f*rightVal;	break;
			case 'f':
			case 'F':
				flyMode = !flyMode;			break;
			case ' ':	
				newPos += vec3(0,2,0);		break;
			case 'p':
			case 'P':
				passiveTurn = !passiveTurn; break;
			default:
				break;
		}
		if(!flyMode || newPos.y <= terrainHeight){
			flyMode = false;
			newPos.y = terrainHeight;
		}
		position.setTarget(newPos);
	}
	void updateSmooth(void){
		position.update(1);
		front.update(1);
		updateRightAndTop();
		updateView();
	}
	void useOrthoProj(bool use){
		useOrthoProjection = use;
	}
	vec3 getLightPose(void) { return lightPos; };
	mat4 getViewMatrix(void) { return viewMatrix; };
	mat4 getProjectionMatrix(void) { return projectionMatrix; };
	vec3 getPosition(void){return position.getVal();};
};

class Program{
		GLuint programID = 0;
		bool shaderReady = false;
		char** loadShaderSource(const char* file) {
			FILE* fp = fopen(file, "rb");
			fseek(fp, 0, SEEK_END);
			long sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			char *src = new char[sz + 1];
			fread(src, sizeof(char), sz, fp);
			src[sz] = '\0';
			char **srcp = new char*[1];
			srcp[0] = src;
			return srcp;
		};
		void shaderLog(GLuint shader, const char* shaderFile) {
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				GLchar* errorLog = new GLchar[maxLength];
				glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
				cout << "=================================================\n";
				cout << "ERROR WHEN COMPILE SHADERFILE -> " << shaderFile << endl;
				printf("%s\n", errorLog);
				cout << "=================================================\n";
				delete[] errorLog;
				exit(1);
			}
			else
			{
				printf("Shader compiled sucess : %s\n", shaderFile);
			}
		};
		void freeShaderSource(char** srcp){
			delete[] srcp[0];
			delete[] srcp;
		};
		GLuint loadShader(GLuint type, const char* shaderFile) {
			GLuint shader = glCreateShader(type);
			char** shaderSource = loadShaderSource(shaderFile);
			glShaderSource(shader, 1, shaderSource, NULL);
			freeShaderSource(shaderSource);
			glCompileShader(shader);
			shaderLog(shader, shaderFile);
			return shader;
		};
	protected:
		void loadShaders(const char* vertexName, const char* fragmentName) {
			GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexName);
			GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentName);
			programID = glCreateProgram();
			glAttachShader(programID, vertexShader);
			glAttachShader(programID, fragmentShader);
			glLinkProgram(programID);
			shaderReady = true;
		}

		void isShaderReady(void){
			if(!shaderReady){
				cout << "Use Program Before Shaders Are Ready !" << endl;
				exit(-1);
			}
		};
	public:
		Program(void){};
		~Program(){};
		void start(void){
			isShaderReady();
			glUseProgram(programID);
		};
		void stop(void){
			isShaderReady();
			glUseProgram(0);
		};
		GLuint getProgramID(void) {
			return programID;
		}
};

#endif
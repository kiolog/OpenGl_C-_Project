

#ifndef INCLUDE_HEADERS
#define INCLUDE_HEADERS	
#pragma once

#include "GLEW/glew.h"
#ifdef _MSC_VER
    #include "FreeGLUT/freeglut.h"
    #include <direct.h>
#else
    #include <GLUT/glut.h>
    #include <unistd.h>
#endif

#include "TinyOBJ/tiny_obj_loader.h"
#include "STB/stb_image.h"
#include "utils/glutils.hpp"
#include "utils/timer.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <functional>
#include <typeinfo>

#ifdef _MSC_VER
    #pragma comment (lib, "glew32.lib")
	#pragma comment(lib, "freeglut.lib")
    #pragma comment(lib, "assimp.lib")
#endif

#define GLM_SWIZZLE
#include "GLM/glm/glm.hpp"
#include "GLM/glm/gtc/matrix_transform.hpp"
#include "GLM/glm/gtc/type_ptr.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>

#ifdef _MSC_VER
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
    #define __FILEPATH__(x) ((std::string(__FILE__).substr(0, std::string(__FILE__).rfind('\\'))+(x)).c_str())
#else
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
    #define __FILEPATH__(x) ((std::string(__FILE__).substr(0, std::string(__FILE__).rfind('/'))+(x)).c_str())
#endif

#ifndef _DEG2RAD
#define _DEG2RAD
	#define M_PI 3.14159f
    #define deg2rad(x) float((float(x))*((M_PI)/(180.0f)))
#endif
#endif


void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}

char** loadShaderSource(const char* file)
{
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
}

void freeShaderSource(char** srcp)
{
	delete[] srcp[0];
	delete[] srcp;
}


GLuint create_program(char* vs, char* fs)
{
	GLuint prog = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource(vs);
	char** fragmentShaderSource = loadShaderSource(fs);
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	shaderLog(vertexShader);
	shaderLog(fragmentShader);
	glAttachShader(prog, vertexShader);
	glAttachShader(prog, fragmentShader);
	glLinkProgram(prog);
	return prog;
}
typedef struct _TextureData
{
	_TextureData(void) :
		width(0),
		height(0),
		data(0)
	{
	}

	int width;
	int height;
	unsigned char* data;
} TextureData;


TextureData loadPNG(const char* const pngFilepath)
{
	TextureData texture;
	int components;

	// load the texture with stb image, force RGBA (4 components required)
	stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);

	// is the image successfully loaded?
	if (data != NULL)
	{
		// copy the raw data
		size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
		texture.data = new unsigned char[dataSize];
		memcpy(texture.data, data, dataSize);

		// mirror the image vertically to comply with OpenGL convention
		for (size_t i = 0; i < texture.width; ++i)
		{
			for (size_t j = 0; j < texture.height / 2; ++j)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					size_t coord1 = (j * texture.width + i) * 4 + k;
					size_t coord2 = ((texture.height - j - 1) * texture.width + i) * 4 + k;
					std::swap(texture.data[coord1], texture.data[coord2]);
				}
			}
		}

		// release the loaded image
		stbi_image_free(data);
	}
	else
	{
		printf("%s Load PNG fail!\n", pngFilepath);
	}

	return texture;
}
typedef struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
};

typedef struct  Material
{
	GLuint diffuse_tex;
};
#ifndef SHADERLIB_H
#define SHADERLIB_H
#include<string>

using namespace std;
using namespace glm;

void AttachShaderSource(int _iProgram, std::string _strFileName, int _iShaderType) {
	if (_strFileName != "") {
		GLuint fs = glCreateShader(_iShaderType);
		char** ShaderSource = loadShaderSource(_strFileName.c_str());
		glShaderSource(fs, 1, ShaderSource, NULL);
		glCompileShader(fs);
		glAttachShader(_iProgram, fs);
		shaderLog(fs);
	}
}
int CreateProgram(std::string _strVertexShader, std::string _strFragmentShader = "", std::string _strTCSShader = "", std::string _strTESShader = "")
{
	int iNewProgram = glCreateProgram();

	char** vertexShaderSource = loadShaderSource(_strVertexShader.c_str());

	//printf("ShaderSource : %s", vertexShaderSource[0]);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vertexShaderSource, NULL);
	glCompileShader(vs);

	glAttachShader(iNewProgram, vs);
	shaderLog(vs);
	AttachShaderSource(iNewProgram, _strFragmentShader, GL_FRAGMENT_SHADER);
	AttachShaderSource(iNewProgram, _strTCSShader, GL_TESS_CONTROL_SHADER);
	AttachShaderSource(iNewProgram, _strTESShader, GL_TESS_EVALUATION_SHADER);
	glLinkProgram(iNewProgram);
	glUseProgram(iNewProgram);
	return iNewProgram;
}
GLuint CreateTexture(string _strPath, vec2 _v2Size = vec2()) {

	GLuint iTextureID;
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	if (_strPath != "") {
		TextureData tdata = loadPNG(_strPath.c_str());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _v2Size.x, _v2Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//printf("TextureID : %d\n", iTextureID);
	return iTextureID;
}

struct FBOProperty {
	GLuint m_iFBO;
	GLuint m_iRBO;
	vector<GLuint> m_ListTexture;
	GLuint m_iDepthTexture;
	vec2 m_v2TextureSize;
};
void DeleteFBO(FBOProperty _FBO) {
	glDeleteRenderbuffers(1, &_FBO.m_iRBO);
	glDeleteFramebuffers(1, &_FBO.m_iFBO);
	int iListCount = _FBO.m_ListTexture.size();
	for (int i = 0; i < iListCount; ++i) {
		glDeleteTextures(1, &_FBO.m_ListTexture[i]);
	}
}
FBOProperty CreateFBO(int width, int height, int _SpecifyTiling = GL_REPEAT, int _iTextureNumber = 1,int _iPixelType = GL_UNSIGNED_BYTE) {
	FBOProperty NewFBO;
	NewFBO.m_v2TextureSize = vec2(width, height);
	glGenFramebuffers(1, &NewFBO.m_iFBO);

	glGenRenderbuffers(1, &NewFBO.m_iRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, NewFBO.m_iRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	// TODO :
	// (1) Generate a texture for FBO
	// (2) Bind it so that we can specify the format of the textrue
	printf("Bind Number : %d\n", _iTextureNumber);
	for (int i = 0; i < _iTextureNumber; ++i) {
		GLuint iNewTextureID;
		glGenTextures(1, &iNewTextureID);
		glBindTexture(GL_TEXTURE_2D, iNewTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, _iPixelType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _SpecifyTiling);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _SpecifyTiling);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// TODO :
		// (1) Bind the framebuffer with first parameter "GL_DRAW_FRAMEBUFFER" 
		// (2) Attach a renderbuffer object to a framebuffer object
		// (3) Attach a texture image to a framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, NewFBO.m_iFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, iNewTextureID, 0);
		NewFBO.m_ListTexture.push_back(iNewTextureID);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, NewFBO.m_iRBO);

	return NewFBO;
}
FBOProperty CreateFBOWithDepthTexture(int width, int height) {
	FBOProperty NewFBO = CreateFBO(width, height);
	GLuint iDepthTexture;
	glBindFramebuffer(GL_FRAMEBUFFER, NewFBO.m_iFBO);
	glGenTextures(1, &iDepthTexture); //Create fboDataTexture
	glBindTexture(GL_TEXTURE_2D, iDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, iDepthTexture, 0);
	NewFBO.m_iDepthTexture = iDepthTexture;

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return NewFBO;
}

void PrintVector3(string _strString, vec3 _Vector) {
	printf("%s (%.3f, %.3f, %.3f)\n", _strString.c_str(), _Vector.x, _Vector.y, _Vector.z);
}
void PrintMat4(string _strString, mat4 _Matrix) {
	printf("%s\n", _strString.c_str());
	for (int i = 0; i < 4; ++i) {
		printf("(%.3f, %.3f, %.3f, %.3f)\n", _Matrix[i][0], _Matrix[i][1], _Matrix[i][2], _Matrix[i][3]);
	}
}

class BoundingBox {
public:
	BoundingBox() {

	}
	BoundingBox(vec3 _v3BasePoint, vec3 _v3Size) {
		m_v3RotateBasePoint = _v3BasePoint;
		m_v3BasePoint = m_v3RotateBasePoint;
		m_v3Size = _v3Size;
		InitVertex();
		m_ListVertexWithRotation = m_ListVertex;
		m_fBoundingSize = length(_v3Size * 0.5f);
	}
	void InitVertex() {
		vec3 v3HalfSize = m_v3Size * 0.5f;
		m_ListVertex.push_back(vec3(-v3HalfSize.x, -v3HalfSize.y, v3HalfSize.z));
		m_ListVertex.push_back(vec3(v3HalfSize.x, -v3HalfSize.y, v3HalfSize.z));
		m_ListVertex.push_back(vec3(v3HalfSize.x, -v3HalfSize.y, -v3HalfSize.z));
		m_ListVertex.push_back(vec3(-v3HalfSize.x, -v3HalfSize.y, -v3HalfSize.z));

		m_ListVertex.push_back(vec3(-v3HalfSize.x, v3HalfSize.y, v3HalfSize.z));
		m_ListVertex.push_back(vec3(v3HalfSize.x, v3HalfSize.y, v3HalfSize.z));
		m_ListVertex.push_back(vec3(v3HalfSize.x, v3HalfSize.y, -v3HalfSize.z));
		m_ListVertex.push_back(vec3(-v3HalfSize.x, v3HalfSize.y, -v3HalfSize.z));
	}
	void ScaleVertex(mat4 _ScaleMatrix) {
		int iListCount = m_ListVertex.size();
		for (int i = 0; i < iListCount; ++i) {
			m_ListVertex[i] = mat3(_ScaleMatrix) * m_ListVertex[i];
		}
	}
	void RotateVertex(mat4 _RotateMatrix) {
		int iListCount = m_ListVertex.size();
		for (int i = 0; i < iListCount; ++i) {
			m_ListVertexWithRotation[i] = mat3(_RotateMatrix) * m_ListVertex[i];
			//PrintVector3("Point : ", m_ListVertex[i]);
			//PrintVector3("Point : ", m_ListVertexWithRotation[i]);
		}
		m_v3RotateBasePoint = mat3(_RotateMatrix) * m_v3BasePoint;
	}
	vec3 m_v3RotateBasePoint;
	vec3 m_v3BasePoint;
	vec3 m_v3Size;
	vector<vec3> m_ListVertex;
	vector<vec3> m_ListVertexWithRotation;
	float m_fBoundingSize;
};

typedef float(*DelGetTerrainHeight) (float _fX, float _fY);

float ComputeRadiansFromTwoVector(vec2 _v2Source, vec2 _v2Target) {
	float fCosineTheta = dot(normalize(_v2Source), normalize(_v2Target));
	float fReturnValue = acos(fCosineTheta);
	if (_v2Target.y < 0) {
		fReturnValue *= -1;
	}
	return fReturnValue;
}



#endif
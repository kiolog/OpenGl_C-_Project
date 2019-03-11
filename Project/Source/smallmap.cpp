#ifndef _SMAllMAP_
#define _SMAllMAP_ 

#include "../Externals/Include/Include.h"
#include "utilities.cpp"
#include "terrain.cpp"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace glm;

class SmallMapCamera : public Camera
{
public:
	SmallMapCamera(){};
	~SmallMapCamera(){};
	void initialize(GLuint texSize) {
		// useOrthoProjection = true;
		position.setVal(vec3(0.0f,350.0f,0.0f));
		front.setVal(vec3(0.0f,-1.0f,0.0f));
		top = vec3(1,0,0);
		right = vec3(0,0,-1);
		updateProj(vec2(texSize,texSize));
		updateView();
	};
};

class smallMapProgram : public Program{
	struct {
		GLuint  scale;
		GLuint	offset;
	}uniforms;
public:
	void initialize(void){
		loadShaders("smallMap_vertex.vs.glsl","smallMap_fragment.fs.glsl");
		uniforms.offset = glGetUniformLocation(getProgramID(),"offset");
		uniforms.scale = glGetUniformLocation(getProgramID(),"scale");
	}
	void loadUniforms(const vec2& offsetVal, const vec2& scaleVal){
		glUniform2fv(uniforms.offset, 1, value_ptr(offsetVal));
		glUniform2fv(uniforms.scale, 1, value_ptr(scaleVal));
	}
	smallMapProgram(){};
	~smallMapProgram(){};
};


class SmallMap
{
	GLuint texSize;
	GLuint textureID;
	GLuint fbo;
	GLuint vao;
	GLuint rbo;
	
	int quadSize;
	int padding;
	vec2 offset;
	vec2 scale;

	SmallMapCamera mapCamera;
	smallMapProgram _program;

public:
	SmallMap(int tSize, int qSize = 100,int pad = 20){texSize = tSize; quadSize = qSize; padding = pad;};
	
	void updateOffset(vec2 win_size){
		offset = vec2(win_size.x - padding - 0.5f * quadSize, padding + 0.5 * quadSize) / win_size;
		printf("%f   %f\n",offset.x,offset.y );
		offset.x = 2 * (offset.x - 0.5); 
		offset.y = 2 * (0.5 - offset.y);
		printf("%f   %f\n\n",offset.x,offset.y );
		scale = vec2(quadSize) / win_size;
	}
	void initialize(void){
		glGenFramebuffers(1, &fbo);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,texSize,texSize);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		_program.initialize();
		mapCamera.initialize(texSize);
		glGenVertexArrays(1, &vao);
	}

	void generateTexture(Terrains& terrain, TerrainProgram& _program, vec2 win_size){
		glViewport(0, 0, texSize, texSize);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			terrain.render(mapCamera, _program, mapCamera.getPosition());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, win_size.x, win_size.y);
	}
	void render(){
		glDisable(GL_DEPTH_TEST);
		_program.start();
			glBindVertexArray(vao);
			_program.loadUniforms(offset, scale);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);
		_program.stop();
		glEnable(GL_DEPTH_TEST);
	}
	SmallMap(){};
	~SmallMap(){};
	
};


#endif



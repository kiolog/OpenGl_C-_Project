#include "../Externals/Include/Include.h"
#include "utilities.cpp"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace glm;


class SnowFallProgram : public Program
{
	struct {
		GLuint projectionMatrix;
		GLuint viewMatrix;
		GLuint modelMatrix;
	}uniforms;
public:
	SnowFallProgram(){};
	~SnowFallProgram(){};
	void initialize(){
		loadShaders("SnowFall.vs.glsl","SnowFall.fs.glsl");
		uniforms.projectionMatrix = glGetUniformLocation(getProgramID(), "projectionMatrix");
		uniforms.viewMatrix		  = glGetUniformLocation(getProgramID(), "viewMatrix");
		uniforms.modelMatrix	  = glGetUniformLocation(getProgramID(), "modelMatrix");
		cout << "Uniform Locations\n";
		cout << "\tprojectionMatrix :" << uniforms.projectionMatrix << endl;
		cout << "\tviewMatrix :" << uniforms.viewMatrix << endl;
		cout << "\tmodelMatrix :"  << uniforms.modelMatrix << endl;
	}
	void loadUniforms(Camera& _camera, mat4 modelMatrix){
		glUniformMatrix4fv(uniforms.projectionMatrix, 1, GL_FALSE, value_ptr(_camera.getProjectionMatrix()));
		glUniformMatrix4fv(uniforms.viewMatrix, 1, GL_FALSE, value_ptr(_camera.getViewMatrix()));
		glUniformMatrix4fv(uniforms.modelMatrix, 1, GL_FALSE, value_ptr(modelMatrix));
	};	
};

class SnowFall
{
	GLuint vao;
	mat4 modelMatrix = mat4(1);
public:
	SnowFall(){};
	void initialize(void){
		glGenVertexArrays(1, &vao);
	}
	void render(Camera& _camera, SnowFallProgram& _program){
		_program.start();
			_program.loadUniforms(_camera, modelMatrix);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glBindVertexArray(0);
		_program.stop();
	}
	~SnowFall(){};
	
};

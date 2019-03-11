#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include"../Include.h"
#include<vector>
#include<string>

using namespace std;
using namespace glm;

class Window {
public:
	Window() {
		m_iProgram = CreateProgram("../Shader/Window_Vertex.shader", "../Shader/Window_Fragment.shader");
		//m_iProgram = CreateProgram("../Shader/FluidSimulate_Vertex.shader", "../Shader/FluidSimulate_Fragment.shader");
		glGenVertexArrays(1, &m_iVao);
	}
	void SetTextureID(GLuint _iTextureID) {
		m_iTextureID = _iTextureID;
	}
	void Render() {
		glUseProgram(m_iProgram);
		glBindVertexArray(m_iVao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iTextureID);
		glDrawArrays(GL_QUADS, 0, 4);
	}
private:
	GLuint m_iProgram;
	GLuint m_iTextureID;
	GLuint m_iVao;
};


#endif
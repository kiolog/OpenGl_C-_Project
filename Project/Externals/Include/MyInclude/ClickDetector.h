#pragma once

#ifndef CLICKDETECTOR_H
#define CLICKDETECTOR_H

#include"../Include.h"
#include<vector>
#include<string>

using namespace std;
using namespace glm;

class ClickDetector {
public:
	ClickDetector() {
		m_iProgram = CreateProgram("../Shader/ClickDetector_Vertex.shader", "../Shader/ClickDetector_Fragment.shader");
		m_iMVP_Location = glGetUniformLocation(m_iProgram, "mvp_matrix");
		m_iModelMatrix_Location = glGetUniformLocation(m_iProgram, "model_matrix");
		m_iScalar_Location = glGetUniformLocation(m_iProgram, "m_fScalar");
	}
	void Reshape(int _iWidth,int _iHeight) {
		DeleteFBO(m_FBO);
		m_FBO = CreateFBO(_iWidth, _iHeight,GL_REPEAT,1, GL_UNSIGNED_SHORT);
		m_v2FBOTextureSize = vec2(_iWidth, _iHeight);
	}
	void SetTerrainSize(float _fSize) {
		m_fScalar = _fSize;
	}
	void InitSetting() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO.m_iFBO);
		ClearDepthAndColor();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(m_iProgram);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	void SettingMatrix(mat4 &_ModelMatrix, mat4 &_ViewMatrix, mat4 &_ProjMatrix) {
		glUniformMatrix4fv(m_iMVP_Location, 1, GL_FALSE, value_ptr(_ProjMatrix * _ViewMatrix * _ModelMatrix));
		glUniformMatrix4fv(m_iModelMatrix_Location, 1, GL_FALSE, value_ptr(_ModelMatrix));
		glUniform1f(m_iScalar_Location, m_fScalar);
		//printf("m_iMVP_Location : %d\n", m_iMVP_Location);
		//printf("m_iModelMatrix_Location : %d\n", m_iModelMatrix_Location);
	}
	GLuint GetTextureID() {
		return m_FBO.m_ListTexture[0];
	}
	float GetScalar() {
		return m_fScalar;
	}
	vec3 GetClickWorldPosition(int _ix,int _iy) {

	}
private:
	void ClearDepthAndColor() {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static const GLfloat green[] = { 0.f, 0.f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}
	FBOProperty m_FBO;
	GLuint m_iProgram;
	vec2 m_v2FBOTextureSize;
	GLuint m_iMVP_Location;
	GLuint m_iModelMatrix_Location;
	GLuint m_iScalar_Location;
	float m_fScalar = 0;
	float m_fInverseScalar;
};


#endif
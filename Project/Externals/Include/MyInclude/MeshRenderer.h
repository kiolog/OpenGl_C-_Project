#pragma once

#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include"../Include.h"
#include<vector>
#include<string>
#include"MyLib.h"

using namespace std;
using namespace glm;

class MeshRenderer {
public:
	MeshRenderer() {
		m_iProgram = CreateProgram("../Shader/BaseModel_Vertex.shader", "../Shader/BaseModel_Fragment.shader");
		m_iModelMatrix_Location = glGetUniformLocation(m_iProgram, "model_matrix");
		m_iViewMatrix_Location = glGetUniformLocation(m_iProgram, "view_matrix");
		m_iProjMatrix_Location = glGetUniformLocation(m_iProgram, "proj_matrix");
		//m_iEyePosition_Location = glGetUniformLocation(m_iProgram, "m_v3EyePosition");
		//m_iTerrainSize_Location = glGetUniformLocation(m_iProgram, "m_v2TerrainSize");
		m_iPlane_Location = glGetUniformLocation(m_iProgram, "m_v4Plane");
		m_iShadowMatrix_Location = glGetUniformLocation(m_iProgram, "shadow_matrix");
		m_iLightPosition_Location = glGetUniformLocation(m_iProgram, "m_v3LightPosition");
		m_iEyePosition_Location = glGetUniformLocation(m_iProgram, "m_v3EyePosition");
	}
	void SetEyePosition(vec3 _v3EyePosition) {
		m_v3EyePosition = _v3EyePosition;
	}
	void SetModelMatrix(mat4 _ModelMatrix) {
		m_ModelMatrix = _ModelMatrix;
	}
	void SetViewMatrix(mat4 _ViewMatrix) {
		m_ViewMatrix = _ViewMatrix;
	}
	void SetProjMatrix(mat4 _ProjMatrix) {
		m_ProjMatrix = _ProjMatrix;
	}
	void SetShadowMapTextureID(GLuint _iTextureID) {
		m_iShadowMapTextuerID = _iTextureID;
	}
	void SetShadowMatrix(mat4 _Matrix) {
		m_ShadowMatrix = _Matrix;
	}
	void SetLightPosition(vec3 _v3Position) {
		m_v3LightPosition = _v3Position;
	}
	void Setting(vec4 _v4Plane) {
		glUseProgram(m_iProgram);
		glEnable(GL_CLIP_DISTANCE0);
		//printf("%f, %f, %f, %f\n", _v4Plane.x, _v4Plane.y, _v4Plane.z, _v4Plane.w);
		glUniformMatrix4fv(m_iModelMatrix_Location, 1, GL_FALSE, value_ptr(m_ModelMatrix));
		glUniformMatrix4fv(m_iViewMatrix_Location, 1, GL_FALSE, value_ptr(m_ViewMatrix));
		glUniformMatrix4fv(m_iProjMatrix_Location, 1, GL_FALSE, value_ptr(m_ProjMatrix));
		glUniformMatrix4fv(m_iShadowMatrix_Location, 1, GL_FALSE, value_ptr(m_ShadowMatrix));
		glUniform3fv(m_iLightPosition_Location, 1, value_ptr(m_v3LightPosition));
		glUniform3fv(m_iEyePosition_Location, 1, value_ptr(m_v3EyePosition));
		//printf("Plane : %f, %f, %f, %f\n", _v4Plane.x, _v4Plane.y, _v4Plane.z, _v4Plane.w);
		glUniform4fv(m_iPlane_Location, 1, value_ptr(_v4Plane));
		//PrintMat4("Matrix", m_ModelMatrix);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_iShadowMapTextuerID);
	}
private:
	GLuint m_iEyePosition_Location;
	GLuint m_iModelMatrix_Location;
	GLuint m_iViewMatrix_Location;
	GLuint m_iProjMatrix_Location;
	GLuint m_iPlane_Location;
	GLuint m_iShadowMatrix_Location;
	GLuint m_iLightPosition_Location;
	GLuint m_iProgram;
	GLuint m_iShadowMapTextuerID;

	mat4 m_ViewMatrix;
	mat4 m_ModelMatrix = mat4(1.0f);
	mat4 m_ProjMatrix;

	vec3 m_v3EyePosition;
	
	mat4 m_ShadowMatrix;
	
	vec3 m_v3LightPosition;
};

#endif
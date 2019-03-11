#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#include"../Include.h"
#include<vector>
#include<string>
#include "MyLib.h"
using namespace std;
using namespace glm;

class SkyBox {
public:
	SkyBox(vec3 _v3Scale) {
	}
	SkyBox(vector<string> _ListFace,vec3 _v3Scale) {
		m_iProgram = CreateProgram("../Shader/SkyBox_vertex.vs.glsl", "../Shader/SkyBox_fragment.fs.glsl");
		m_iMVP_Location = glGetUniformLocation(m_iProgram, "mvp");
		m_ModelMatrix = scale(mat4(), _v3Scale);
		//printf("SkyBox\n");
		GLuint iBufferData = 0;
		glGenVertexArrays(1, &m_iVao);
		glBindVertexArray(m_iVao);
		glEnableVertexAttribArray(0);
		glGenBuffers(1, &iBufferData);
		glBindBuffer(GL_ARRAY_BUFFER, iBufferData);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_ListVertex), m_ListVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		GLuint iIndexData;

		glGenBuffers(1, &iIndexData);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIndexData);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_ListIndex), m_ListIndex, GL_STATIC_DRAW);

		LoadCubeMap(_ListFace);
	}
	void LoadCubeMap(vector<string> _ListFace) {
		printf("Load Cube Map\n");
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glGenTextures(1, &m_iTextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_iTextureID);
		TextureData NewTexture;
		int iListCount = _ListFace.size();
		for (unsigned int i = 0; i < iListCount; ++i)
		{
			NewTexture = loadPNG(_ListFace[i].c_str());
			printf("%d %d\n", NewTexture.width, NewTexture.height);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, NewTexture.width, NewTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewTexture.data);

		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	void SetProjMatrix(mat4 _ProjMatrix) {
		m_ProjMatrix = _ProjMatrix;
	}
	void SetViewMatrix(mat4 _ViewMatrix) {
		m_ViewMatrix = _ViewMatrix;
	}
	void Render() {
		glUseProgram(m_iProgram);

		mat4 mvp = m_ProjMatrix * m_ViewMatrix * m_ModelMatrix;
		glUniformMatrix4fv(m_iMVP_Location, 1, GL_FALSE, &mvp[0][0]);
		//printf("Render\n");
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_iVao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_iTextureID);
		glDrawElements(GL_QUADS, sizeof(m_ListIndex) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	}
	GLuint GetTextureID() {
		return m_iTextureID;
	}
private:
	GLuint m_iProgram;
	GLuint m_iMVP_Location;
	GLuint m_iVao;
	GLuint m_iTextureID;
	mat4 m_ProjMatrix;
	mat4 m_ViewMatrix;
	mat4 m_ModelMatrix;
	vec3 m_v3Scale;
	float m_ListVertex[24] = {
		-1.0,  1.0,  1.0,
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
	};
	GLushort m_ListIndex[24] = {
		0, 1, 2, 3,
		3, 2, 6, 7,
		7, 6, 5, 4,
		4, 5, 1, 0,
		0, 3, 7, 4,
		1, 2, 6, 5,
	};
	
	
};
#endif
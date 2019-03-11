#pragma once

#ifndef ANIMALMENU_H
#define ANIMALMENU_H

#include"../Include.h"
#include<vector>
#include<string>
#include"AnimalManager.h"
using namespace std;
using namespace glm;

struct MenuItem {
	GLuint m_iNormalTextureID;
	GLuint m_iHoverTextureID;
	EAnimalType m_AnimalType;
};

class AnimalMenu {
public:
	AnimalMenu() {
		glGenVertexArrays(1, &m_iVao);
		m_iProgram = CreateProgram("../Shader/GUI_Vertex.shader", "../Shader/GUI_Fragment.shader");
		m_iSize_Location = glGetUniformLocation(m_iProgram, "m_v2Size");
		m_iOrigin_Location = glGetUniformLocation(m_iProgram, "m_v2Origin");
		printf("Size Location : %d\n", m_iSize_Location);
		printf("Origin  Location : %d\n", m_iOrigin_Location);
		m_iBorder_Selected = CreateTexture("../Media/Texture/Border_Selected.png");
		m_iBorder_Normal = CreateTexture("../Media/Texture/Border_Normal.png");
		AddMenuItem(WOLF, "../Media/Texture/Wolf_Normal.png", "../Media/Texture/Wolf_Hover.png");
		AddMenuItem(COW, "../Media/Texture/Cow_Normal.png", "../Media/Texture/Cow_Hover.png");
	}
	void Reshape(int _iX,int _iY) {
		int iMinValue = glm::min(_iX, _iY);
		if (_iX <= _iY) {
			m_v2Size = vec2(m_fBaseSize, m_fBaseSize * ((float)_iX / _iY));
			m_v2Padding = vec2(m_fBasePadding, m_fBasePadding * ((float)_iX / _iY));
		}
		else {
			m_v2Size = vec2(m_fBaseSize * ((float)_iY / _iX), m_fBaseSize);
			m_v2Padding = vec2(m_fBasePadding * ((float)_iY / _iX), m_fBasePadding);
		}
		m_fPadding = m_fBasePadding * iMinValue;
		m_v2ScreenSize = vec2(_iX, _iY);
	}
	void SetMousePosition(vec2 _v2MousePosition) {
		m_v2MousePosition = _v2MousePosition / m_v2ScreenSize;
	}
	bool MouseClick(int _iX, int _iY) {
		bool bReturnValue = false;
		vec2 v2BasePoint = m_v2Padding + m_v2Size * 0.5f;
		vec2 v2HalfSize = m_v2Size * 0.5f;
		int iListCount = m_ListMenuItem.size();
		for (int i = 0; i < iListCount; ++i) {
			vec2 v2ScreenPoint = vec2(v2BasePoint.x + i * (m_v2Padding.x + m_v2Size.x), v2BasePoint.y);
			vec2 v2RelativePoint = m_v2MousePosition - v2ScreenPoint;
			if (v2RelativePoint.x >= -v2HalfSize.x && v2RelativePoint.x <= v2HalfSize.x && v2RelativePoint.y >= -v2HalfSize.y && v2RelativePoint.y <= v2HalfSize.y) {
				m_iClickIndex = i;
				bReturnValue = true;
				break;
			}
		}
		return bReturnValue;
	}
	EAnimalType GetSelectedAnimalType() {
		//printf("Click Index : %d\n", m_iClickIndex);
		return m_ListMenuItem[m_iClickIndex].m_AnimalType;
	}
	void Render() {
		glClear(GL_DEPTH_BUFFER_BIT);
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(m_iProgram);
		glEnable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(m_iVao);
		vec2 v2BasePoint = m_v2Padding + m_v2Size * 0.5f;
		vec2 v2HalfSize = m_v2Size * 0.5f;
		glUniform2fv(m_iSize_Location, 1, value_ptr(m_v2Size * 2.f));
		int iListCount = m_ListMenuItem.size();
		for (int i = 0; i < iListCount; ++i) {
			glUniform2fv(m_iSize_Location, 1, value_ptr(m_v2Size * 2.f));
			vec2 v2ScreenPoint = vec2(v2BasePoint.x + i * (m_v2Padding.x + m_v2Size.x), v2BasePoint.y);
			vec2 v2ClipPoint = vec2(-1, 1) + vec2(v2BasePoint.x + i * (m_v2Padding.x + m_v2Size.x), -v2BasePoint.y) * 2.f;
			vec2 v2RelativePoint = m_v2MousePosition - v2ScreenPoint;
			//printf("m_v2MousePosition : (%f, %f)\n", m_v2MousePosition.x, m_v2MousePosition.y);
			//printf("v2ScreenPoint : (%f, %f)\n", v2ScreenPoint.x, v2ScreenPoint.y);
			//printf("Relative Point : (%f, %f)\n", v2RelativePoint.x, v2RelativePoint.y);
			if (v2RelativePoint.x >= -v2HalfSize.x && v2RelativePoint.x <= v2HalfSize.x && v2RelativePoint.y >= -v2HalfSize.y && v2RelativePoint.y <= v2HalfSize.y) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_ListMenuItem[i].m_iHoverTextureID);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_ListMenuItem[i].m_iNormalTextureID);
			}
			//printf("Point : (%f, %f)\n", v2Point.x, v2Point.y);
			glUniform2fv(m_iOrigin_Location, 1, value_ptr(v2ClipPoint));
			glDrawArrays(GL_QUADS, 0, 4);

			glUniform2fv(m_iSize_Location, 1, value_ptr(m_v2Size * 2.2f));
			if (m_iClickIndex == i) {

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_iBorder_Selected);
				glDrawArrays(GL_QUADS, 0, 4);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_iBorder_Normal);
			}
		}
	}
	void UpdateKeyBoardDown(const char _Key) {
		switch (_Key) {
		case '1':case '2':
			m_iClickIndex = _Key - '1';
			break;
		}
	}
private:
	void AddMenuItem(EAnimalType _AnimalType,string _strNormalPath,string _strHoverPath) {
		MenuItem NewMenuItem;
		NewMenuItem.m_iNormalTextureID = CreateTexture(_strNormalPath);
		NewMenuItem.m_iHoverTextureID = CreateTexture(_strHoverPath);
		NewMenuItem.m_AnimalType = _AnimalType;
		m_ListMenuItem.push_back(NewMenuItem);
	}
	GLuint m_iVao;
	GLuint m_iBorder_Selected;
	GLuint m_iBorder_Normal;
	GLuint m_iProgram;
	GLuint m_iSize_Location;
	GLuint m_iOrigin_Location;
	vector<MenuItem> m_ListMenuItem;
	vec2 m_v2MousePosition;
	vec2 m_v2Size;
	vec2 m_v2Padding;
	float m_fPadding;
	float m_fBaseSize = 0.15f;
	float m_fBasePadding = 0.03f;
	vec2 m_v2ScreenSize;
	int m_iClickIndex = 0;
};


#endif
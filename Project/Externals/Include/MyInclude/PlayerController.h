#pragma once

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include"../Include.h"
#include<vector>
#include<string>
#include "GameObject.h"
#include"MyLib.h"
using namespace std;
using namespace glm;

class PlayerController {
public:
	void AddGameObject(GameObject *_GameObject) {
		m_ListGameObject.push_back(_GameObject);
	}
	virtual void UpdatePosition(float _fDeltaTime) {

	}
	vector<GameObject*> GetListGameObject() {
		return m_ListGameObject;
	}
	void SetGetTerrainHeight(std::function<float(float, float,float)> _Function) {
		m_GetTerrainHeight = _Function;
	}
	void PrintHeight(float _fWidth, float _fHeight) {
		//printf("Terrain Height : %f\n", m_GetTerrainHeight(_fWidth, _fHeight));
	}
	void RemoveObject(GameObject *_GameObject) {
		
		RemoveObjectFromVector(m_ListGameObject, _GameObject);
		//printf("Size : %d\n", m_ListGameObject.size());
	}
protected:
	vector<GameObject*> m_ListGameObject;
	std::function<float(float, float,float)> m_GetTerrainHeight;
};

#endif
#pragma once
#ifndef MYLIB_H
#define MYLIB_H

#include"../Include.h"
#include<vector>
#include<string>
#include"GameObject.h"
using namespace std;
using namespace glm;

void RemoveObjectFromVector(vector<GameObject*> &_ListGameObject, GameObject *_RemoveGameObject) {
	int iListCount = _ListGameObject.size();
	for (int i = 0; i < iListCount; ++i) {
		if (_ListGameObject[i] == _RemoveGameObject) {
			_ListGameObject.erase(_ListGameObject.begin() + i);
			break;
		}
	}
}

#endif

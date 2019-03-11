#pragma once

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H


#include<vector>
#include<string>
#include "GameObject.h"
#include "FirstPersonCamera.h"
#include "PlayerController.h"

using namespace std;
using namespace glm;

class KeyBoardController :public PlayerController {
public:
	KeyBoardController() {

	}
	KeyBoardController(vector<char> _ListKey) {
		m_ListKey = _ListKey;
		m_v3MoveDirection = vec3();
		m_fRotateYaw = 0;
	}
	void UpdatePosition(float _fDeltaTime) override {
		//PrintVector3("MoveDirection", m_v3MoveDirection);
		//printf("Rotate Yaw", m_fRotateYaw);
		int iListCount = m_ListGameObject.size();
		
		for (int i = 0; i < iListCount; ++i) {
			GameObject *TraceGameObject = m_ListGameObject[i];
			TraceGameObject->UpdatePosition(m_v3MoveDirection, _fDeltaTime);
			vec3 v3NextPosition = TraceGameObject->GetNextPosition();
			v3NextPosition.y = m_GetTerrainHeight(v3NextPosition.x, v3NextPosition.z, TraceGameObject->GetWorldPosition().y);
			TraceGameObject->SetNextPosition(v3NextPosition);
			//PrintVector3("Next Position : ", v3NextPosition);
			//TraceGameObject->SetWorldPosition(v3NextPosition);
			//TraceGameObject->UpdateModelMatrix();
		}
	}
	void UpdateKeyDown(const char _Key) {
		if (_Key == m_ListKey[0]) {
			m_v3MoveDirection.z += 1;
		}
		else if (_Key == m_ListKey[1]) {
			m_v3MoveDirection.z += -1;
		}
		else if (_Key == m_ListKey[2]) {
			m_v3MoveDirection.x += -1;
		}
		else if (_Key == m_ListKey[3]) {
			m_v3MoveDirection.x += 1;
		}
		else if (_Key == m_ListKey[4]) {
			m_fRotateYaw += 1;
		}
		else if (_Key == m_ListKey[5]) {
			m_fRotateYaw += -1;
		}
	}
	void UpdateKeyUp(const char _Key) {
		if (_Key == m_ListKey[0]) {
			m_v3MoveDirection.z += -1;
		}
		else if (_Key == m_ListKey[1]) {
			m_v3MoveDirection.z += 1;
		}
		else if (_Key == m_ListKey[2]) {
			m_v3MoveDirection.x += 1;
		}
		else if (_Key == m_ListKey[3]) {
			m_v3MoveDirection.x += -1;
		}
		else if (_Key == m_ListKey[4]) {
			m_fRotateYaw += -1;
		}
		else if (_Key == m_ListKey[5]) {
			m_fRotateYaw += 1;
		}
	}
private:
	void UpdateMoveDirection() {

	}
	
	vec3 m_v3MoveDirection;
	float m_fRotateYaw;
	vector<char> m_ListKey;
};

#endif
#pragma once

#ifndef AICONTROLLER_H
#define AICONTROLLER_H


#include<vector>
#include<string>
#include "GameObject.h"
#include "PlayerController.h"
#include <time.h>
using namespace std;
using namespace glm;
class AIProperty {
public:
	AIProperty(){

	}
	AIProperty(GameObject *_GameObject, vec2 _v2BasePoint, float _fRadius) {
		m_GameObject = _GameObject;
		m_v2BasePoint = _v2BasePoint;
		m_fRadius = _fRadius;
	}
	GameObject *m_GameObject;
	vec2 m_v2MoveDirection = vec2(1,0);
	vec3 m_v3TargetPosition;
	vec2 m_v2BasePoint;
	float m_fRadius;
	int m_iAngle = 0;
};
class AIController :public PlayerController {
public:
	AIController() {
		srand(time(NULL));
	}
	void AddGameObject(GameObject *_GameObject,vec2 _v2BasePoint, float _fRadius) {
		AIProperty *NewAIProperty = new AIProperty(_GameObject, _v2BasePoint, _fRadius);
		NewAIProperty->m_v3TargetPosition = _GameObject->GetWorldPosition();
		//UpdateNextTarget(NewAIProperty);
		m_ListAIProperty.push_back(NewAIProperty);
		m_ListGameObject.push_back(_GameObject);
	}
	void UpdatePosition(float _fDeltaTime) override {
		//PrintVector3("MoveDirection", m_v3MoveDirection);
		//printf("Rotate Yaw", m_fRotateYaw);
		int iListCount = m_ListAIProperty.size();
		//printf("Size : %d\n", iListCount);
		for (int i = 0; i < iListCount; ++i) {
			AIProperty *TraceAIProperty = m_ListAIProperty[i];
			UpdateEachAI(TraceAIProperty, _fDeltaTime);
		}
	}
	void RemoveObject2(GameObject *_GameObject){
		//PlayerController::RemoveObject(_GameObject);
		int iListCount = m_ListAIProperty.size();
		for (int i = 0; i < iListCount; ++i) {
			AIProperty *TraceAIProperty = m_ListAIProperty[i];
			if (TraceAIProperty->m_GameObject == _GameObject) {
				m_ListAIProperty.erase(m_ListAIProperty.begin() + i);
				//printf("Delete Object : %d\n", i);
				break;
			}
		}
		PlayerController::RemoveObject(_GameObject);
		//printf("Size : %d\n", m_ListAIProperty.size());
		//PlayerController::RemoveObject(_GameObject);
		//RemoveObjectFromVector(m_ListGameObject, _GameObject);
	}
	
	
private:
	void UpdateEachAI(AIProperty *_AIProperty,float _fDeltaTime) {
		GameObject *TraceGameObject = _AIProperty->m_GameObject;
		if (TraceGameObject->GetCollidedObjectType() == OBSTACLE || length(TraceGameObject->GetWorldPosition() - _AIProperty->m_v3TargetPosition) <= m_fTolerantDistance) {
			//printf("Arrive!!\n");
			//printf("Collided Type : %d\n", TraceGameObject->GetCollidedObjectType());
			UpdateNextTarget(_AIProperty);
		}
		else {
			//PrintVector3("Target Position : ", _AIProperty->m_v3TargetPosition);
			//PrintVector3("World Position : ", TraceGameObject->GetWorldPosition());
			vec2 v2DeltaMove = _AIProperty->m_v2MoveDirection * _fDeltaTime * TraceGameObject->GetMoveSpeed();
			//printf("Move Direction (%f, %f)\n", v2DeltaMove.x, v2DeltaMove.y);
			vec2 v2NextPosition = TraceGameObject->GetWorldPosition().xz + _AIProperty->m_v2MoveDirection * _fDeltaTime * TraceGameObject->GetMoveSpeed();
			vec3 v3NextPosition = vec3(v2NextPosition.x, m_GetTerrainHeight(v2NextPosition.x, v2NextPosition.y, TraceGameObject->GetWorldPosition().y), v2NextPosition.y);
			TraceGameObject->SetNextPosition(v3NextPosition);
			//PrintVector3("Next Position : ", v3NextPosition);
		}
	}
	void UpdateNextTarget(AIProperty *_AIProperty) {
		int iAngle = (rand() % 180) + 90 + _AIProperty->m_iAngle;
		vec2 v2BasePoint = _AIProperty->m_v2BasePoint;
		float fRadius = _AIProperty->m_fRadius;
		GameObject *TraceGameObject = _AIProperty->m_GameObject;

		mat4 RotationMatrix = rotate(mat4(), deg2rad(iAngle), vec3(0, 1, 0));
		vec3 v3Direction = mat3(RotationMatrix) * vec3(1, 0, 0);
		//PrintVector3("Direction : ", v3Direction);
		float fDistance = fRadius * ((rand() % 100) / 100.f);
		vec3 v3TargetPosition = v3Direction * fDistance + vec3(v2BasePoint.x, 0, v2BasePoint.y);
		v3TargetPosition.y = m_GetTerrainHeight(v3TargetPosition.x, v3TargetPosition.y, TraceGameObject->GetWorldPosition().y);
		//printf("Select Yaw : %f\n", deg2rad(iAngle));
		vec2 v2MoveDirection = normalize(v3TargetPosition.xz - TraceGameObject->GetWorldPosition().xz);
		float fAngle = ComputeRadiansFromTwoVector(vec2(1, 0), v2MoveDirection);
		TraceGameObject->SetNextYaw(fAngle);
		_AIProperty->m_v3TargetPosition = v3TargetPosition;
		_AIProperty->m_v2MoveDirection = v2MoveDirection;
		_AIProperty->m_iAngle = iAngle;
	}
	vector<AIProperty*> m_ListAIProperty;
	float m_fTolerantDistance = 5.f;
};

#endif
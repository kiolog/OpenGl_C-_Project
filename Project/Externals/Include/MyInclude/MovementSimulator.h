#pragma once

#ifndef MOVEMENTSIMULATOR_H
#define MOVEMENTSIMULATOR_H

#include"../Include.h"
#include"PlayerController.h"
#include"KeyBoardController.h"
#include"GameObject.h"
#include<vector>
#include<string>
#include"CollisionEngine.h"
#include "AIController.h"
#include"MyLib.h"
using namespace std;
using namespace glm;

class MovementSimulator {
public:
	MovementSimulator(std::function<float(float, float,float)> _Function,vec2 _v2MapBounding) {
		m_v2MapBounding = _v2MapBounding;
		m_GetTerrainHeight = _Function;
		m_AIController = new AIController();
		dynamic_cast<PlayerController*>(m_AIController)->SetGetTerrainHeight(m_GetTerrainHeight);
		m_ListPlayerController.push_back(m_AIController);
		++m_iControllerCount;
	}
	void AddPlayerController(PlayerController* _PlayerController) {
		if (dynamic_cast<KeyBoardController*>(_PlayerController) != NULL) {
			m_ListKeyBoardController.push_back(dynamic_cast<KeyBoardController*>(_PlayerController));
			printf("Add KeyBoard Controller!!!\n");
		}
		_PlayerController->SetGetTerrainHeight(m_GetTerrainHeight);
		m_ListPlayerController.push_back(_PlayerController);
		++m_iControllerCount; 
		//printf("Move Size : %d\n", m_ListMoveGameObject.size());
		//printf("Insert Size : %d\n", _PlayerController->GetListGameObject().size());
		vector<GameObject*> ListAppendVector = _PlayerController->GetListGameObject();
		m_ListMoveGameObject.insert(m_ListMoveGameObject.end(), ListAppendVector.begin(), ListAppendVector.end());
	}
	void AddToAIController(GameObject *_GameObject,vec2 _v2BasePoint,float _fRadius) {
		m_AIController->AddGameObject(_GameObject, _v2BasePoint, _fRadius);
		m_ListMoveGameObject.push_back(_GameObject);
		m_ListGameObject.push_back(_GameObject);
	}
	void AddGameObject(GameObject *_GameObject) {
		m_ListGameObject.push_back(_GameObject);
	}
	void Update(float _fDeltaTime) {
		UpdateNextPosition(_fDeltaTime);
		UpdateCollision();
	}
	void UpdateKeyBoardDown(unsigned char key, int x, int y) {
		int iListCount = m_ListKeyBoardController.size();
		for (int i = 0; i < iListCount; ++i) {
			m_ListKeyBoardController[i]->UpdateKeyDown(key);
		}
	}
	void UpdateKeyBoardUp(unsigned char key, int x, int y) {
		int iListCount = m_ListKeyBoardController.size();
		for (int i = 0; i < iListCount; ++i) {
			m_ListKeyBoardController[i]->UpdateKeyUp(key);
		}
	}
	void SetGetTerrainHeight(std::function<float(float, float,float)> _Function) {
		m_GetTerrainHeight = _Function;
	}
	void RemoveObject(GameObject *_GameObject) {
		RemoveObjectFromVector(m_ListGameObject, _GameObject);
		RemoveObjectFromVector(m_ListMoveGameObject, _GameObject);
		int iListCount = m_ListPlayerController.size();
		for (int i = 0; i < iListCount; ++i) {
			m_ListPlayerController[i]->RemoveObject(_GameObject);
		}
	}
private:
	void UpdateNextPosition(float _fDeltaTime) {
		//printf("Size : %d\n", m_iControllerCount);
		for (int i = 0; i < m_iControllerCount; ++i) {
			m_ListPlayerController[i]->UpdatePosition(_fDeltaTime);
		}
	}
	void UpdateCollision() {
		int iListCount = m_ListMoveGameObject.size();
		for (int i = 0; i < iListCount; ++i) {
			GameObject *TraceGameObject = m_ListMoveGameObject[i];
			bool bIsCollided = CheckCollide(TraceGameObject);
			if (!bIsCollided) {
				//PrintVector3("Before Position : ", TraceGameObject->GetWorldPosition());
				TraceGameObject->SetYaw(TraceGameObject->GetNextYaw());
				TraceGameObject->SetWorldPosition(TraceGameObject->GetNextPosition());
				TraceGameObject->UpdateModelMatrix();
				//PrintVector3("After Next Position : ", TraceGameObject->GetNextPosition());
				//PrintVector3("After Position : ", TraceGameObject->GetWorldPosition());
				//PrintVector3("Simulator Next Position : ", TraceGameObject->GetNextPosition());
			}
			else {
				//printf("Collide\n");
				//PrintVector3("Before Position : ", TraceGameObject->GetWorldPosition());
				//PrintVector3("After Position : ", TraceGameObject->GetNextPosition());
				TraceGameObject->RefreshPositionAndYaw();
			}
			//printf("Is Collide? : %d\n", bIsCollided);
		}
	}
	bool CheckCollide(GameObject *_GameObject) {
		_GameObject->SetCollidedObjectType(NONE);
		bool bReturnValue = false;
		BoundingBox *SourceBoundingBox = _GameObject->GetBoundingBox();
		vec3 v3SourcePosition = _GameObject->GetNextPosition() + SourceBoundingBox->m_v3RotateBasePoint;

		if (v3SourcePosition.x > -m_v2MapBounding.x && v3SourcePosition.x < m_v2MapBounding.x && v3SourcePosition.z > -m_v2MapBounding.y && v3SourcePosition.z < m_v2MapBounding.y) {
			vector<vec3> ListSourceVertex = SourceBoundingBox->m_ListVertexWithRotation;
			int iListCount = m_ListGameObject.size();
			for (int i = 0; i < iListCount; ++i) {
				GameObject *TraceGameObject = m_ListGameObject[i];
				if (_GameObject != TraceGameObject) {
					BoundingBox *TargetBoundingBox = TraceGameObject->GetBoundingBox();
					vec3 v3TargetPosition = TraceGameObject->GetWorldPosition() + TargetBoundingBox->m_v3RotateBasePoint;
					vector<vec3> ListTargetVertex = TargetBoundingBox->m_ListVertexWithRotation;
					float fDistance = length(v3TargetPosition - v3SourcePosition);
					if (fDistance <= SourceBoundingBox->m_fBoundingSize + TargetBoundingBox->m_fBoundingSize) {
						bReturnValue = GetGJKResult(v3SourcePosition, ListSourceVertex, v3TargetPosition, ListTargetVertex);
						if (bReturnValue == true) {
							if (_GameObject->IsFirstCollided()) {
								vec2 v2NextPosition = v3TargetPosition.xz + normalize(v3SourcePosition.xz - v3TargetPosition.xz) * (_GameObject->GetBoundingBox()->m_fBoundingSize + TraceGameObject->GetBoundingBox()->m_fBoundingSize);
								//printf("Is First Collided!!!!!!!!!!!\n");
								_GameObject->SetCollidedObjectType(OBSTACLE);
								_GameObject->SetWorldPosition(vec3(v2NextPosition.x, m_GetTerrainHeight(v2NextPosition.x, v2NextPosition.y, _GameObject->GetWorldPosition().y), v2NextPosition.y));
							}
							else {
								_GameObject->SetCollidedObjectType(TraceGameObject->GetCollisionType());
							}
							break;
						}
					}
				}
			}
		}
		else {
			bReturnValue = true;
			_GameObject->SetCollidedObjectType(OBSTACLE);
			PrintVector3("Position : ", v3SourcePosition);
			printf("OutSide!\n");
		}
		_GameObject->SetIsFirstCollided(false);
		
		return bReturnValue;
	}
	vector<KeyBoardController*> m_ListKeyBoardController;
	vector<PlayerController*> m_ListPlayerController;
	vector<GameObject*> m_ListGameObject;
	vector<GameObject*> m_ListMoveGameObject;
	AIController *m_AIController;
	std::function<float(float, float,float )> m_GetTerrainHeight;
	int m_iControllerCount = 0;
	vec2 m_v2MapBounding;
};


#endif
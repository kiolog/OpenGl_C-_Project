#pragma once

#ifndef ANIMALMANAGER_H
#define ANIMALMANAGER_H

#include"../Include.h"
#include "GameObject.h"
#include<vector>
#include<string>
#include "MyFbxMesh.h"
#include"MyLib.h"
using namespace std;
using namespace glm;

enum EAnimalType {
	WOLF = 0,
	COW,
};

struct AnimalProperty {
	AnimalProperty() {

	}
	AnimalProperty(vec3 _v3BaseScale,vec3 _v3BaseTranslation,vec3 _v3RotationParameter,float _fMoveSpeed,float _fAnimationSpeed,vec3 _v3BoundingBoxPosition,vec3 _v3BoundingBoxSize,MyFbxMesh *_FBXMesh) {
		m_v3BaseScale = _v3BaseScale;
		m_v3BaseTranslation = _v3BaseTranslation;
		m_v3RotationParameter = _v3RotationParameter;
		m_fMoveSpeed = _fMoveSpeed;
		m_fAnimationSpeed = _fAnimationSpeed;
		m_FBXMesh = _FBXMesh;
		m_v3BoundingBoxPosition = _v3BoundingBoxPosition;
		m_v3BoundingBoxSize = _v3BoundingBoxSize;
	}
	vec3 m_v3BaseScale;
	vec3 m_v3BaseTranslation;
	vec3 m_v3RotationParameter;
	float m_fMoveSpeed;
	float m_fAnimationSpeed;
	float m_fLifeTime = 10;
	MyFbxMesh *m_FBXMesh;
	vec3 m_v3BoundingBoxPosition;
	vec3 m_v3BoundingBoxSize;
};

class AnimalManager {
public:
	AnimalManager() {
		AddAnimal(WOLF, "../Model/Wolf/WolfModel_Walking.fbx", vec3(2), vec3(0, 3.5f, 0), vec3(radians(-90.f), 0, radians(-90.f)), 5, 2.f, vec3(0, 3.5f, 0), vec3(10));
		AddAnimal(COW, "../Model/Cow/Cow_Walking.fbx", vec3(2), vec3(0, 4.f, 0), vec3(0, radians(-90.f), 0), 5, 3.f, vec3(0, 3.5f, 0), vec3(15, 10, 10));
	}
	void AddAnimal(EAnimalType _AnimalType, string _strFileName, vec3 _v3BaseScale, vec3 _v3BaseTranslation, vec3 _v3RotationParameter, float _fMoveSpeed, float _fAnimationSpeed,vec3 _v3BoundingBoxPosition,vec3 _v3BoundingBoxSize) {
		MyFbxMesh *NewFBXMesh = new MyFbxMesh(_strFileName);
		AnimalProperty NewProperty(_v3BaseScale, _v3BaseTranslation, _v3RotationParameter, _fMoveSpeed, _fAnimationSpeed, _v3BoundingBoxPosition, _v3BoundingBoxSize, NewFBXMesh);
		m_AnimalPropertyMap[_AnimalType] = NewProperty;
	}
	GameObject *CreateAnimal(EAnimalType _AnimalType) {
		AnimalProperty MyProperty = m_AnimalPropertyMap[_AnimalType];
		MyFbxMesh *TraceFbxMesh = MyProperty.m_FBXMesh;
		GameObject *NewGameObject = new MyFbxMesh(TraceFbxMesh->GetShape(), TraceFbxMesh->GetMaterial(), TraceFbxMesh->GetFBX(), MyProperty.m_fAnimationSpeed);
		NewGameObject->SetBaseYawAndPitchAndRoll(MyProperty.m_v3RotationParameter.x, MyProperty.m_v3RotationParameter.y, MyProperty.m_v3RotationParameter.z);
		NewGameObject->SetBaseTranslation(MyProperty.m_v3BaseTranslation);
		NewGameObject->SetBaseScale(MyProperty.m_v3BaseScale);
		NewGameObject->SetCollisionType(OBSTACLE);
		NewGameObject->AddBoundingBox(MyProperty.m_v3BoundingBoxPosition, MyProperty.m_v3BoundingBoxSize);
		NewGameObject->SetMoveSpeed(MyProperty.m_fMoveSpeed);
		NewGameObject->SetLifeTime(MyProperty.m_fLifeTime);
		m_ListGameObject.push_back(NewGameObject);
		return NewGameObject;
	}
	vector<GameObject*> GetAllObject() {
		return m_ListGameObject;
	}
	void RemoveObject(GameObject *_GameObject) {
		RemoveObjectFromVector(m_ListGameObject, _GameObject);
		_GameObject->Delete();
	}
private:
	void Init() {

	}
	map<EAnimalType, AnimalProperty> m_AnimalPropertyMap;
	vector<AnimalProperty> m_ListAnimalProperty;
	vector<GameObject*> m_ListGameObject;
};


#endif
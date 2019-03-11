#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include"../Include.h"
#include<vector>
#include<string>

using namespace std;
using namespace glm;

enum EObjectCollisionType {
	OBSTACLE = 0,
	NORMAL,
	NONE,
};

enum EObjectState {
	ALIVE = 0,
	DIE,
};

class GameObject {
public:
	GameObject() {
		m_ModelMatrix = mat4(1.0f);
		m_fYaw = 0;
	}
	GameObject(float _fMoveSpeed) {
		m_fMoveSpeed = _fMoveSpeed;
	}
	void AddBoundingBox(vec3 _v3BasePoint, vec3 _v3Size) {
		m_BoundingBox = new BoundingBox(_v3BasePoint, _v3Size);
	}
	BoundingBox *GetBoundingBox() {
		return m_BoundingBox;
	}
	void SetCollisionType(EObjectCollisionType _Type) {
		m_CollisionType = _Type;
	}
	void SetCollidedObjectType(EObjectCollisionType _Type) {
		m_CollidedObjectType = _Type;
	}
	void SetModelMatrix(mat4 _Matrix) {
		m_ModelMatrix = _Matrix;
	}
	void SetLifeTime(float _fValue) {
		m_fLifeTime = _fValue;
	}
	void SetBaseYawAndPitchAndRoll(float _fYaw,float _fPitch = 0,float _fRoll = 0) {
		float fYaw = _fYaw;
		float fPitch = _fPitch;
		//printf("Init Yaw : %f\n", _fYaw);
		//printf("Init Pitch : %f\n", _fPitch);
		mat4 YawMatrix = rotate(mat4(), fYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 PitchMatrix = rotate(mat4(), fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 RollMatrix = rotate(mat4(), _fRoll, glm::vec3(0.0f, 0.0f, 1.0f));
		m_BaseRotateMatrix = RollMatrix * PitchMatrix * YawMatrix;
	}
	void SetBaseTranslation(vec3 _v3Translation) {
		m_BaseTranslationMatrix = translate(mat4(), _v3Translation);
	}
	void SetBaseScale(vec3 _v3Scale) {
		m_BaseScaleMatrix = scale(mat4(), _v3Scale);
	}
	vec3 GetWorldPosition() {
		return m_v3WorldPosition;
	}
	vec3 GetNextPosition() {
		return m_v3NextPosition;
	}
	mat4 GetModelMatrix() {
		return m_ModelMatrix;
	}
	void SetForwardDirection(vec3 _v3Direction) {
		m_v3ForwardVector = _v3Direction;
	}
	void SetRightDirection(vec3 _v3Direction) {
		m_v3RightVector = _v3Direction;
	}
	vec3 GetFowardVector() {
		return m_v3ForwardVector;
	}
	vec3 GetRightVector() {
		return m_v3RightVector;
	}
	float GetYaw() {
		return m_fYaw;
	}
	float GetNextYaw() {
		return m_fNextYaw;
	}
	float GetPitch() {
		return m_fPitch;
	}
	EObjectCollisionType GetCollisionType() {
		return m_CollisionType;
	}
	EObjectCollisionType GetCollidedObjectType() {
		return m_CollidedObjectType;
	}
	void SetYaw(float _fYaw) {
		m_fYaw = _fYaw;
		m_BoundingBox->RotateVertex(rotate(mat4(), _fYaw, vec3(0, 1, 0)));
	}
	void SetNextYaw(float _fYaw) {
		m_fNextYaw = _fYaw;
		m_BoundingBox->RotateVertex(rotate(mat4(), _fYaw, vec3(0, 1, 0)));
	}
	void SetMoveSpeed(float _fMoveSpeed) {
		m_fMoveSpeed = _fMoveSpeed;
	}
	void RefreshPositionAndYaw() {
		m_fNextYaw = m_fYaw;
		m_v3NextPosition = m_v3WorldPosition;
	}
	void UpdateLifeTime(float _fDeltaTime) {
		m_fAge += _fDeltaTime;
		if (m_fAge >= m_fLifeTime) {
			m_ObjectState = DIE;
		}
	}
	EObjectState GetObjectState() {
		return m_ObjectState;
	}
	void UpdatePosition(vec3 _v3MoveDirection, float _fDeltaTime) {
		vec3 v3ForwardDirection(m_DynamicModelMatrix[0][0], m_DynamicModelMatrix[0][1], m_DynamicModelMatrix[0][2]);
		vec3 v3RightDirection(m_DynamicModelMatrix[2][0], m_DynamicModelMatrix[2][1], m_DynamicModelMatrix[2][2]);
		m_v3ForwardVector = v3ForwardDirection;
		m_v3RightVector = v3RightDirection;
		//printf("Rotate Yaw : %f\n", _fYaw);
		//PrintVector3("Forward Vector", m_v3ForwardVector);
		//PrintVector3("Right Vector", m_v3RightVector);
		//PrintVector3("Move Direction", _v3MoveDirection);
		m_v3NextPosition = m_v3WorldPosition + (_v3MoveDirection.z * v3ForwardDirection + _v3MoveDirection.x * v3RightDirection + _v3MoveDirection.y * vec3(0, 1, 0)) * _fDeltaTime * m_fMoveSpeed;
		//m_v3WorldPosition += (_v3MoveDirection.z * v3ForwardDirection + _v3MoveDirection.x * v3RightDirection + _v3MoveDirection.y * vec3(0, 1, 0)) * _fDeltaTime * m_fMoveSpeed;
		
	}
	float GetMoveSpeed() {
		return m_fMoveSpeed;
	}
	void SetWorldPosition(vec3 _v3Position) {
		m_v3WorldPosition = _v3Position;
	}
	void SetNextPosition(vec3 _v3Position) {
		m_v3NextPosition = _v3Position;
	}
	void UpdateModelMatrix() {
		mat4 YawMatrix = rotate(mat4(), -m_fYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		//printf("Yaw : %f\n", m_fYaw);
		//mat4 PitchMatrix = rotate(mat4(), -m_fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		//printf("Pitch : %f\n", m_fYaw);
		//order matters
		mat4 RotateMatrix = YawMatrix;
		//PrintMat4("RotateMatrix", RotateMatrix);
		mat4 TranslateMatrix = glm::translate(mat4(), m_v3WorldPosition);
		//PrintMat4("TranslateMatrix", TranslateMatrix);
		m_DynamicModelMatrix = TranslateMatrix * RotateMatrix;
		m_ModelMatrix = m_DynamicModelMatrix *  m_BaseTranslationMatrix * m_BaseRotateMatrix * m_BaseScaleMatrix;
		//PrintMat4("ModelMatrix", m_BaseModelMatrix);
	}
	bool IsFirstCollided() {
		return m_bIsFirstCollided;
	}
	void SetIsFirstCollided(bool _bValue) {
		m_bIsFirstCollided = _bValue;
	}
	virtual void Delete() {

	}
	virtual void Render() {

	}
	virtual void Render(mat4 &_ViewMatrix, mat4 &_ProjMatrix) {

	}
protected:
	vec3 m_v3WorldPosition;
	vec3 m_v3NextPosition;
	mat4 m_ModelMatrix = mat4(1.0f);
	vec3 m_v3ForwardVector;
	vec3 m_v3RightVector;
	float m_fRotateSensitive = 0.01f;
	float m_fYaw = 0;
	float m_fNextYaw = 0;
	float m_fPitch = 0;
	float m_fMoveSpeed = 5;
	mat4 m_BaseRotateMatrix;
	mat4 m_DynamicModelMatrix;
	mat4 m_BaseTranslationMatrix;
	mat4 m_BaseScaleMatrix = mat4();
	BoundingBox *m_BoundingBox;
	EObjectCollisionType m_CollisionType = NORMAL;
	EObjectCollisionType m_CollidedObjectType = NONE;
	bool m_bIsFirstCollided = true;
	float m_fLifeTime;
	float m_fAge = 0;
	EObjectState m_ObjectState = ALIVE;
};

#endif // !MESH_H
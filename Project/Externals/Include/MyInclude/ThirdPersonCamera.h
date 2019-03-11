#pragma once

#ifndef THIRDPERSONCAMERA_H
#define THIRDPERSONCAMERA_H

#include"../Include.h"
#include<vector>
#include<string>
#include "GameObject.h"
#include "BaseCamera.h"
using namespace std;
using namespace glm;

class ThirdPersonCamera :public BaseCamera{
public:
	ThirdPersonCamera(){

	}
	void SetFollowObject(GameObject *_GameObject,vec3 _v3Distance) {
		m_FollowGameObject = _GameObject;
		m_v3Distance = _v3Distance;
		m_BaseRotateMatrix = rotate(mat4(), deg2rad(90), glm::vec3(0.0f, 1.0f, 0.0f));
		//m_fYaw = _GameObject->GetYaw();
		//m_fPitch = _GameObject->GetPitch();
	}
	vec3 GetEyePosition() override {
		return m_v3EyePosition;
	}
	virtual vec3 GetForwardDirection() override{
		return m_v3ForwardVector;
	}
	void UpdateView(float _fDeltaTime) override{
		vec3 v3Position = m_FollowGameObject->GetWorldPosition();
		vec3 v3ObjectPosition = v3Position;
		m_FollowGameObject->SetNextYaw(m_fYaw);
		//m_FollowGameObject->UpdateModelMatrix();
		vec3 v3BackVector = -m_FollowGameObject->GetFowardVector();
		vec3 v3RightVector = m_FollowGameObject->GetRightVector();
		/*PrintVector3("Before Position : ", v3Position);
		PrintVector3("BackVector : ", v3BackVector);
		PrintVector3("RightVector : ", v3RightVector);*/
		vec3 v3ForwardDirection(m_ViewMatrix[0][2], m_ViewMatrix[1][2], m_ViewMatrix[2][2]);
		vec3 v3RightDirection(m_ViewMatrix[0][0], m_ViewMatrix[1][0], m_ViewMatrix[2][0]);
		m_v3ForwardVector = v3ForwardDirection;
		m_v3RightVector = v3RightDirection;
		v3Position += (m_v3Distance.z * v3BackVector + m_v3Distance.x * v3RightVector + m_v3Distance.y * vec3(0, 1, 0));
		//PrintVector3("After Position : ", v3Position);
		mat4 PitchMatrix = rotate(mat4(), m_fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 YawMatrix = rotate(mat4(), m_fYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 RotateMatrix = PitchMatrix * YawMatrix;
		mat4 TranslateMatrix = glm::translate(mat4(), -v3Position);
		m_ViewMatrix = RotateMatrix * m_BaseRotateMatrix *  TranslateMatrix;
		//m_ViewMatrix = lookAt(v3Position, v3ObjectPosition, vec3(0, 1, 0));
		m_v3EyePosition = v3Position;
	}
	mat4 GetFlipViewMatrix(vec4 &_v4Plane) override{
		mat4 PitchMatrix = rotate(mat4(), -m_fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 YawMatrix = rotate(mat4(), m_fYaw, glm::vec3(0.0f, 1.0f, 0.0f));

		//order matters
		mat4 RotateMatrix = PitchMatrix * YawMatrix;
		float fDistance = dot(vec4(m_v3EyePosition, 1), _v4Plane);
		mat4 TranslateMatrix = glm::translate(mat4(), -(m_v3EyePosition - vec3(0, fDistance * 2, 0)));
		return RotateMatrix * m_BaseRotateMatrix * TranslateMatrix;
	}
private:
	GameObject * m_FollowGameObject;
	vec3 m_v3Distance;
	mat4 m_BaseRotateMatrix;
};


#endif
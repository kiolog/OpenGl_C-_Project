#pragma once

#ifndef BASECAMERA_H
#define BASECAMERA_H

#include"../Include.h"
#include"GameObject.h"
#include<vector>
#include<string>

class BaseCamera {
public:
	BaseCamera(mat4 &_ViewMatrix = mat4(), vec3 _v3EyePosition = vec3(), float _fPitch = 0, float _fYaw = 0) 
	:
		m_ViewMatrix(_ViewMatrix),
		m_v3EyePosition(_v3EyePosition),
		m_fPitch(_fPitch),
		m_fYaw(_fYaw)
	{

	}
	virtual void SetFollowObject(GameObject *_GameObject, vec3 _v3Distance = vec3(0,0,0)) {}
	virtual void KeyBoardUp(const char _key) {}
	virtual void KeyBoard(const char _key) {}
	virtual void UpdateView(float _fDeltaTime) {}
	virtual mat4 GetFlipViewMatrix(vec4 &_v4Plane) {
		return mat4();
	}
	
	
	virtual vec3 GetEyePosition() {
		return vec3();
	}
	virtual vec3 GetForwardDirection() {
		return vec3();
	}
	mat4 GetViewMatrix() {
		return m_ViewMatrix;
	}
	void SetOldMousePosition(int _iX, int _iY) {
		m_v2OldMousePoisition = vec2(_iX, _iY);
	}
	void MouseMotion(int _iX, int _iY) {
		glm::vec2 mouse_delta = glm::vec2(_iX, _iY) - m_v2OldMousePoisition;

		//note that yaw and pitch must be converted to radians.
		//this is done in UpdateView() by glm::rotate
		m_fYaw -= -m_fRotateSensitive * mouse_delta.x;
		m_fPitch += -m_fRotateSensitive * mouse_delta.y;

		m_v2OldMousePoisition = glm::vec2(_iX, _iY);
	}
	void PrintParameter() {
		printf("--------------------\n");
		PrintMat4("ViewMatrix", m_ViewMatrix);
		PrintVector3("Eye Vector : ", m_v3EyePosition);
		printf("Pitch : %f\n", m_fPitch);
		printf("Yaw : %f\n", m_fYaw);
		PrintVector3("Forward : ", m_v3ForwardVector);
	}
protected:
	
	vec3 m_v3EyePosition = vec3();
	vec3 m_v3ForwardVector = vec3();
	vec3 m_v3RightVector = vec3();
	mat4 m_ViewMatrix = mat4(1.0f);
	vec2 m_v2OldMousePoisition = vec2(0, 0);
	float m_fPitch = 0;
	float m_fYaw = 0;
	float m_fRotateSensitive = 0.01f;
};



#endif
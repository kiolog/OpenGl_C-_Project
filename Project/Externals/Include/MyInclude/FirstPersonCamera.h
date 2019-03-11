#pragma once

#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include"../Include.h"
#include<vector>
#include<string>
#include"MyLib.h"
#include"BaseCamera.h"

using namespace std;
using namespace glm;

class FirstPersonCamera :public BaseCamera{
public:
	FirstPersonCamera(float _fMoveSpeed, mat4 &_ViewMatrix = mat4(), vec3 _v3EyeVector = vec3(), float _fPitch = 0, float _fYaw = 0)
		:BaseCamera(_ViewMatrix, _v3EyeVector, _fPitch, _fYaw)
	{
		m_fMoveSpeed = _fMoveSpeed;
	}
	void UpdateView(float _fDeltaTime) override {
		vec3 v3ForwardDirection(m_ViewMatrix[0][2], m_ViewMatrix[1][2], m_ViewMatrix[2][2]);
		vec3 v3RightDirection(m_ViewMatrix[0][0], m_ViewMatrix[1][0], m_ViewMatrix[2][0]);
		m_v3ForwardVector = v3ForwardDirection;
		//printf("Forward Direction : %f, %f, %f\n", m_ViewMatrix[0][2], m_ViewMatrix[1][2], m_ViewMatrix[2][2]);

		m_v3EyePosition += (m_v3MoveDirection.z * v3ForwardDirection + m_v3MoveDirection.x * -v3RightDirection + m_v3MoveDirection.y * vec3(0, 1, 0)) * _fDeltaTime * m_fMoveSpeed;


		mat4 PitchMatrix = rotate(mat4(), m_fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 YawMatrix = rotate(mat4(), m_fYaw, glm::vec3(0.0f, 1.0f, 0.0f));

		//order matters
		mat4 RotateMatrix = PitchMatrix * YawMatrix;

		mat4 TranslateMatrix = glm::translate(mat4(), m_v3EyePosition);
		if (m_bIsFlipped) {
			m_ViewMatrix = GetFlipViewMatrix(vec4(0,1,0,0));
		}
		else {
			m_ViewMatrix = RotateMatrix * TranslateMatrix;
		}
	}
	mat4 GetFlipViewMatrix(vec4 &_v4Plane) override {
		mat4 PitchMatrix = rotate(mat4(), -m_fPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 YawMatrix = rotate(mat4(), m_fYaw, glm::vec3(0.0f, 1.0f, 0.0f));

		//order matters
		mat4 RotateMatrix = PitchMatrix * YawMatrix;
		float fDistance = dot(vec4(-m_v3EyePosition, 1), _v4Plane);
		mat4 TranslateMatrix = glm::translate(mat4(), m_v3EyePosition + vec3(0, fDistance * 2, 0));
		//mat4 TranslateMatrix = glm::translate(mat4(), m_v3EyePosition);

		return RotateMatrix * TranslateMatrix;
	}
	vec3 GetEyePosition() override {
		return -m_v3EyePosition;
	}
	vec3 GetForwardDirection() override {
		return -normalize(m_v3ForwardVector);
	}
	void KeyBoard(const char _key)override {
		//printf("key : %c\n", _key);
		switch (_key)
		{
		case 'w':
		{
			m_v3MoveDirection.z += 1;
			break;
		}

		case 's':
		{
			m_v3MoveDirection.z += -1;
			break;
		}
		case 'a':
		{
			m_v3MoveDirection.x += -1;
			break;
		}

		case 'd':
		{
			m_v3MoveDirection.x += 1;
			break;
		}
		case 'p':
		{
			//printf("Location : %f,%f,%f\n", m_v3EyePosition.x, m_v3EyePosition.y, m_v3EyePosition.z);
			break;
		}
		case 'z':
			m_v3MoveDirection.y += -1;
			break;
		case 'x':
			m_v3MoveDirection.y += 1;
			break;
		case 'l':
			PrintParameter();
			break;
		case 'f':
			m_bIsFlipped = true;
			break;
		default:
			break;
		}
		//printf("MoveDirection : ( %f, %f, %f)\n", m_v3MoveDirection.x, m_v3MoveDirection.y, m_v3MoveDirection.z);
	}
	void KeyBoardUp(const char _key)override {
		//printf("key : %c\n", _key);
		switch (_key)
		{
		case 'w':
		{
			m_v3MoveDirection.z += -1;
			break;
		}

		case 's':
		{
			m_v3MoveDirection.z += 1;
			break;
		}
		case 'a':
		{
			m_v3MoveDirection.x += 1;
			break;
		}

		case 'd':
		{
			m_v3MoveDirection.x += -1;
			break;
		}
		case 'p':
		{
			//printf("Location : %f,%f,%f\n", m_v3EyePosition.x, m_v3EyePosition.y, m_v3EyePosition.z);
			break;
		}
		case 'z':
			m_v3MoveDirection.y += 1;
			break;
		case 'x':
			m_v3MoveDirection.y += -1;
			break;
		default:
			break;
		}
		//printf("MoveDirection : ( %f, %f, %f)\n", m_v3MoveDirection.x, m_v3MoveDirection.y, m_v3MoveDirection.z);
	}
private:
	float m_fMoveSpeed = 10;
	vec3 m_v3MoveDirection = vec3();
	bool m_bIsFlipped = false;
};

#endif
#pragma once

#ifndef FLUID_H
#define FLUID_H

#include"../Include.h"
#include<vector>
#include<string>
#include"MyLib.h"
using namespace std;
using namespace glm;


class Fluid {
public:
	Fluid(int _iWidth,int _iHeight,vec3 _v3Position,vec2 _v2Scale = vec2(1,1)) {
		m_v2TerrainSize = vec2(_iWidth, _iHeight);
		glGenVertexArrays(1, &m_iVao);
		m_ModelMatrix = translate(mat4(), _v3Position) * scale(mat4(), vec3(_v2Scale.x, 1, _v2Scale.y));
		m_InverseModelMatrix = inverse(m_ModelMatrix);
		InitFBO(m_v2FBOTextureSize.x, m_v2FBOTextureSize.y);
		InitTesslationShader();
		InitDrawHitPointShader();
		InitFluidSimulationShader();
		InitFluidHeightMap();
		InitFluidNoiseMap();
		InitPlane();
		InitSimplePlaneShader();
	}
	void SetShadowMatrix(mat4 _Matrix) {
		m_ShadowMatrix = _Matrix;
	}
	void InitPlane() {
		vec3 v3Normal = normalize(mat3(m_ModelMatrix) * vec3(0, 1, 0));
		vec3 v3WorldPoint = (m_ModelMatrix * vec4(0, 0, 0, 1)).xyz;
		float fDValue = -dot(v3Normal, v3WorldPoint);
		m_v4Plane = vec4(v3Normal, fDValue);
		printf("Init Plane : %f, %f, %f, %f\n", m_v4Plane.x, m_v4Plane.y, m_v4Plane.z, m_v4Plane.w);
	}
	vec4 GetPlane() {
		return m_v4Plane;
	}
	vec4 GetRefractionPlane() {
		vec4 ReturnVector = vec4(m_v4Plane.x, -m_v4Plane.y, m_v4Plane.z, -(m_v4Plane.w - 8));
		//printf("Reverse Plane : %f, %f, %f, %f\n", ReturnVector.x, ReturnVector.y, ReturnVector.z, ReturnVector.w);
		return ReturnVector;
	}
	void Reshape(int _iWidth, int _iHeight) {
		m_v2WindowSize = vec2(_iWidth, _iHeight);
	}
	void InitFBO(int _iWidth,int _iHeight) {
		m_FBOArray[0] = CreateFBO(_iWidth, _iHeight, GL_CLAMP_TO_EDGE);
		m_FBOArray[1] = CreateFBO(_iWidth, _iHeight, GL_CLAMP_TO_EDGE);
		m_FBOArray[2] = CreateFBO(_iWidth, _iHeight, GL_CLAMP_TO_EDGE);
		m_iFBOArrayIndex = 0;
		m_UsedFBO = m_FBOArray[0];

		m_ReflectFBO = CreateFBO(_iWidth, _iHeight);
		m_RefractionFBO = CreateFBOWithDepthTexture(_iWidth, _iHeight);
		//printf("Depth Texture : %d\n", m_RefractionFBO.m_iDepthTexture);
	}
	void InitFluidSimulationShader() {
		m_iFluidSimulationProgram = CreateProgram("../Shader/FluidSimulate_Vertex.shader", "../Shader/FluidSimulate_Fragment.shader");
	}
	void InitTesslationShader() {
		m_iTesselationProgram = CreateProgram("../Shader/Fluid_Vertex.shader", "../Shader/Fluid_Fragment.shader", "../Shader/Tesselation_TCS.shader", "../Shader/Tesselation_TES.shader");
		m_iModelMatrix_Location = glGetUniformLocation(m_iTesselationProgram, "model_matrix");
		m_iViewMatrix_Location = glGetUniformLocation(m_iTesselationProgram, "view_matrix");
		m_iProjMatrix_Location = glGetUniformLocation(m_iTesselationProgram, "proj_matrix");
		m_iEyePosition_Location = glGetUniformLocation(m_iTesselationProgram, "m_v3EyePosition");
		m_iTerrainSize_Location = glGetUniformLocation(m_iTesselationProgram, "m_v2TerrainSize");
		m_iMoveFactor_Location = glGetUniformLocation(m_iTesselationProgram, "m_fMoveFactor");
		m_iTime_Location = glGetUniformLocation(m_iTesselationProgram, "m_fTime");
		m_iPlane_Location = glGetUniformLocation(m_iTesselationProgram, "m_v4Plane");
		m_iViewingDirection_Location = glGetUniformLocation(m_iTesselationProgram, "m_v3ViewingDirection");
		m_iHeightMapPixelSize_Location = glGetUniformLocation(m_iTesselationProgram, "m_v2HeightMapPixelSize");
		m_iShadowMatrix_Location = glGetUniformLocation(m_iTesselationProgram, "shadow_matrix");
		m_iLightPosition_Location = glGetUniformLocation(m_iTesselationProgram, "m_v3LightPosition");

		m_iHeightMapID = CreateTexture("../Media/Texture/terragen.png");
		m_iTextureID = CreateTexture("../Media/Texture/WaterTexture_1.png");
		m_iDUDVTextureID = CreateTexture("../Media/Texture/waterDUDV.png");
		m_iNormalMapTexture = CreateTexture("../Media/Texture/WaterNormalMap_2.png");

		m_v2HeightMapPixelSize = vec2(1, 1) / m_v2FBOTextureSize;

		printf("InitFluid\n");
	}
	void InitSimplePlaneShader() {
		m_iSimplePlaneProgram = CreateProgram("../Shader/SimplePlane_Vertex.shader", "../Shader/SimplePlane_Fragment.shader");
		m_iSimplePlaneMVP_Location = glGetUniformLocation(m_iSimplePlaneProgram, "mvp_matrix");
		m_iSimplePlaneTerrainSize_Location = glGetUniformLocation(m_iSimplePlaneProgram, "m_v2TerrainSize");
	}
	void InitDrawHitPointShader() {
		m_iDrawHitPointProgram = CreateProgram("../Shader/DrawHitPoint_Vertex.shader", "../Shader/DrawHitPoint_Fragment.shader");
		m_iHitPosition_Location = glGetUniformLocation(m_iDrawHitPointProgram, "m_v2HitPosition");
		m_iScale_Location = glGetUniformLocation(m_iDrawHitPointProgram, "m_fPointScale");
		m_iHitOrNot_Location = glGetUniformLocation(m_iDrawHitPointProgram, "m_bHit");

		m_iPointTextureID = CreateTexture("../Media/Texture/gaus2d.png");
	}
	void InitFluidNoiseMap() {
		m_iNoiseMapProgram = CreateProgram("../Shader/DrawHitPoint_Vertex.shader", "../Shader/FluidNoiseMap_Fragment.shader");
		m_iNoiseMapTime_Location = glGetUniformLocation(m_iNoiseMapProgram, "m_fTime");
		m_NoiseMapFBO = CreateFBO(m_v2FBOTextureSize.x, m_v2FBOTextureSize.y);
	}
	void InitFluidHeightMap() {
		m_iFluidHeightMapID = CreateTexture("", vec2(1024, 1024));
	}
	void SetEyePosition(vec3 _v3EyePosition) {
		m_v3EyePosition = _v3EyePosition;
	}
	void SetViewMatrix(mat4 _ViewMatrix) {
		m_ViewMatrix = _ViewMatrix;
	}
	void SetProjMatrix(mat4 _ProjMatrix) {
		m_ProjMatrix = _ProjMatrix;
	}
	void SetViewingDirection(vec3 _v3Direction) {
		m_v3ViewingDirection = _v3Direction;
	}
	void SetCameraPitch(float _fPitch) {
		m_fFlipPitch = -_fPitch;
	}
	void SetShadowMapTextureID(GLuint _iTextureID) {
		m_iShadowMapTextureID = _iTextureID;
	}
	void SetLightPosition(vec3 _v3Position) {
		m_v3LightPosition = _v3Position;
	}
	void CheckHit(vec3 _v3Position,vec3 _v3Direction,float _fDistance) {
		printf("CheckHit\n");
		vec3 v3Normal = mat3(m_ModelMatrix) * vec3(0, 1, 0);
		vec3 v3FacePoint = (m_ModelMatrix * vec4(0, 0, 0, 1)).xyz;
		float fDValue = GetValueD(v3FacePoint, v3Normal, _v3Position, _v3Direction);
		if (fDValue != -1) {
			if (fDValue >= 0 && fDValue <= _fDistance) {
				vec3 v3HitWorldPosition = _v3Position + fDValue * _v3Direction;
				PrintVector3("Hit World Position : ", v3HitWorldPosition);
				vec3 v3HitPosition = (m_InverseModelMatrix * vec4(v3HitWorldPosition, 1)).xyz;
				PrintVector3("Hit Local Position : ", v3HitPosition);
				vec2 v2TextureCoord = ((v3HitPosition.xz + m_v2TerrainSize * 0.5f) / m_v2TerrainSize);
				if (v2TextureCoord.x < 0 || v2TextureCoord.x > 1 || v2TextureCoord.y < 0 || v2TextureCoord.y > 1) {
				}
				else {
					m_bIsHit = 1;
					m_v2HitPosition = v2TextureCoord;
				}
			}
		}
		/*printf("Face Normal : %f, %f, %f\n", v3Normal.x, v3Normal.y, v3Normal.z);
		printf("View Direction : %f, %f, %f\n", _v3Direction.x, _v3Direction.y, _v3Direction.z);
		printf("DValue : %f\n", fDValue);*/
		if (m_bIsHit) {
			printf("Is Hit!!!\n");
			printf("m_v2HitPosition : %f, %f\n", m_v2HitPosition.x, m_v2HitPosition.y);
		}
		else {
			printf("Not Hit\n");
		}

	}
	float GetValueD(vec3 _v3FacePoint,vec3 _v3FaceNormal,vec3 _v3LineStartPoint,vec3 _v3LineDirection) {
		float fReturnValue = -1;
		float fDenominator = dot(_v3FaceNormal, _v3LineDirection);
		if (fDenominator != 0) {
			float fNumerator = dot((_v3FacePoint - _v3LineStartPoint), _v3FaceNormal);
			fReturnValue = fNumerator / fDenominator;
		}
		return fReturnValue;
	}
	void UpdateDrawHitPointShader() {
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_UsedFBO.m_iFBO);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glUseProgram(m_iDrawHitPointProgram);
		glBindVertexArray(m_iVao);
		glUniform2fv(m_iHitPosition_Location, 1, value_ptr(m_v2HitPosition));
		glUniform1i(m_iHitOrNot_Location, m_bIsHit);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iPointTextureID);
		glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, GetLastTexture(m_iFBOArrayIndex, 1));
		glBindTexture(GL_TEXTURE_2D, m_UsedFBO.m_ListTexture[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		m_bIsHit = 0;
		
	}
	void UpdateFluidSimulation() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_UsedFBO.m_iFBO);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glUseProgram(m_iFluidSimulationProgram);
		glBindVertexArray(m_iVao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetLastTexture(m_iFBOArrayIndex, 1));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GetLastTexture(m_iFBOArrayIndex, 2));

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	void UpdateNoiseMap() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_NoiseMapFBO.m_iFBO);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glUseProgram(m_iNoiseMapProgram);
		glBindVertexArray(m_iVao);

		glUniform1f(m_iNoiseMapTime_Location, m_iTime);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	GLuint GetLastTexture(int _iBaseIndex, int _iOffset) {
		int iIndex = _iBaseIndex - _iOffset;
		if (iIndex < 0) {
			iIndex += 3;
		}
		//printf("Index : %d\n", iIndex);
		return m_FBOArray[iIndex].m_ListTexture[0];
	}
	void DrawTesslationShader() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, m_v2WindowSize[0], m_v2WindowSize[1]);
		glUseProgram(m_iTesselationProgram);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(m_iVao);
		//PrintMat4("ModelMatrix", m_ModelMatrix);
		glUniformMatrix4fv(m_iModelMatrix_Location, 1, GL_FALSE, value_ptr(m_ModelMatrix));
		glUniformMatrix4fv(m_iViewMatrix_Location, 1, GL_FALSE, value_ptr(m_ViewMatrix));
		glUniformMatrix4fv(m_iProjMatrix_Location, 1, GL_FALSE, value_ptr(m_ProjMatrix));
		glUniformMatrix4fv(m_iShadowMatrix_Location, 1, GL_FALSE, value_ptr(m_ShadowMatrix));
		glUniform2fv(m_iTerrainSize_Location, 1, value_ptr(m_v2TerrainSize));
		glUniform3fv(m_iEyePosition_Location, 1, value_ptr(m_v3EyePosition));
		glUniform3fv(m_iViewingDirection_Location, 1, value_ptr(m_v3ViewingDirection));
		glUniform3fv(m_iLightPosition_Location, 1, value_ptr(m_v3LightPosition));
		glUniform4fv(m_iPlane_Location, 1, value_ptr(m_v4Plane));
		glUniform1f(m_iTime_Location, m_iTime);
		glUniform1f(m_iMoveFactor_Location, m_fMoveFactor);
		glUniform2fv(m_iHeightMapPixelSize_Location, 1, value_ptr(m_v2HeightMapPixelSize));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_UsedFBO.m_ListTexture[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_iTextureID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_ReflectFBO.m_ListTexture[0]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_RefractionFBO.m_ListTexture[0]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_iDUDVTextureID);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_iNormalMapTexture);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_RefractionFBO.m_iDepthTexture);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, m_iShadowMapTextureID);

		glDrawArraysInstanced(GL_PATCHES, 0, 4, m_v2TerrainSize.x * m_v2TerrainSize.y);
	}
	void TextureCycleMovement() {
		m_iFBOArrayIndex = (m_iFBOArrayIndex + 1) % 3;
		m_UsedFBO = m_FBOArray[m_iFBOArrayIndex];
	}
	void ClearDepthAndColor() {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static const GLfloat green[] = { 0.f, 0.f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}
	void ReflectionSetting() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_ReflectFBO.m_iFBO);
		ClearDepthAndColor();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CLIP_DISTANCE0);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	void RefractionSetting() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_RefractionFBO.m_iFBO);
		ClearDepthAndColor();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(4.0f, 4.0f);
		glEnable(GL_CLIP_DISTANCE0);
		glViewport(0, 0, m_v2FBOTextureSize[0], m_v2FBOTextureSize[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glDrawBuffer(GL_DEPTH_ATTACHMENT);
	}
	void SimpleRender(mat4 _VPMatrix) {
		glUseProgram(m_iSimplePlaneProgram);
		glBindVertexArray(m_iVao);
		glUniformMatrix4fv(m_iSimplePlaneMVP_Location, 1, GL_FALSE, value_ptr(_VPMatrix * m_ModelMatrix));
		glUniform2fv(m_iSimplePlaneTerrainSize_Location, 1, value_ptr(m_v2TerrainSize));

		//printf("Index : %d\n", m_iSimplePlaneMVP_Location);
		//printf("Index2 : %d\n", m_iSimplePlaneTerrainSize_Location);

		glDrawArrays(GL_QUADS, 0, 4);
	}
	void Render() {
		if (m_fMoveFactor >= 1.f) {
			m_fMoveFactor = 0;
		}
		m_fMoveFactor += m_fMoveSpeed;
		//printf("MoveFactor : %f\n", m_fMoveFactor);
		//ClearDepthAndColor();
		//UpdateNoiseMap();
		UpdateDrawHitPointShader();
		TextureCycleMovement();
		//ClearDepthAndColor();
		UpdateFluidSimulation();
		//ClearDepthAndColor();
		DrawTesslationShader();
		//glEnable(GL_CLIP_DISTANCE0);
		//printf("%f, %f, %f, %f\n", m_v4Plane.x, m_v4Plane.y, m_v4Plane.z, m_v4Plane.w);
	}
	void AddGameObject(GameObject *_GameObject) {
		m_ListNonCollidedGameObject.push_back(_GameObject);
	}
	void RemoveObject(GameObject *_GameObject) {
		RemoveObjectFromVector(m_ListNonCollidedGameObject, _GameObject);
		RemoveObjectFromVector(m_ListCollidedGameObject, _GameObject);
	}
	void UpdateCollision() {
		UpdateNonCollidedObject();
		UpdateCollidedObject();
	}
private:
	void UpdateNonCollidedObject() {
		int iListCount = m_ListNonCollidedGameObject.size();
		for (int i = 0; i < iListCount; ++i) {
			GameObject *TraceGameObject = m_ListNonCollidedGameObject[i];
			if (TraceGameObject->GetWorldPosition().y <= -m_v4Plane.w) {
				vec3 v3HitWorldPosition = vec3(TraceGameObject->GetWorldPosition().x, -m_v4Plane.w, TraceGameObject->GetWorldPosition().z);
				vec3 v3HitPosition = (m_InverseModelMatrix * vec4(v3HitWorldPosition, 1)).xyz;
				PrintVector3("Hit Local Position : ", v3HitPosition);
				vec2 v2TextureCoord = ((v3HitPosition.xz + m_v2TerrainSize * 0.5f) / m_v2TerrainSize);
				if (v2TextureCoord.x < 0 || v2TextureCoord.x > 1 || v2TextureCoord.y < 0 || v2TextureCoord.y > 1) {
				}
				else {
					m_bIsHit = 1;
					m_v2HitPosition = v2TextureCoord;
					m_ListCollidedGameObject.push_back(TraceGameObject);
					m_ListNonCollidedGameObject.erase(m_ListNonCollidedGameObject.begin() + i);
					break;
				}
			}
		}
	}
	void UpdateCollidedObject() {
		int iListCount = m_ListCollidedGameObject.size();
		//printf("Size : %d\n", iListCount);
		for (int i = iListCount - 1; i >= 0; --i) {
			GameObject *TraceGameObject = m_ListCollidedGameObject[i];
			if (TraceGameObject->GetWorldPosition().y > -m_v4Plane.w) {
				
				m_ListNonCollidedGameObject.push_back(TraceGameObject);
				m_ListCollidedGameObject.erase(m_ListCollidedGameObject.begin() + i);
			}
		}
	}
	FBOProperty m_UsedFBO;

	GLuint m_iEyePosition_Location;
	GLuint m_iTerrainSize_Location;
	GLuint m_iModelMatrix_Location;
	GLuint m_iViewMatrix_Location;
	GLuint m_iProjMatrix_Location;
	GLuint m_iTime_Location;
	GLuint m_iTextureID;
	GLuint m_iHeightMapID;
	GLuint m_iFluidHeightMapID;
	GLuint m_iVao;
	GLuint m_iTesselationProgram;
	vec2 m_v2TerrainSize;
	mat4 m_ViewMatrix;
	mat4 m_ModelMatrix;
	mat4 m_ProjMatrix;

	vec3 m_v3EyePosition;

	//DrawHitPointShader
	GLuint m_iDrawHitPointProgram;
	GLuint m_iHitPosition_Location;
	GLuint m_iHitOrNot_Location;
	GLuint m_iScale_Location;
	GLuint m_iPointTextureID;
	int m_bIsHit = 0;
	vec2 m_v2HitPosition;

	//Fluid
	GLuint m_iFluidSimulationProgram;

	FBOProperty m_FBOArray[3];
	int m_iFBOArrayIndex = 0;

	vec2 m_v2FBOTextureSize = vec2(2000, 2000);
	vec2 m_v2WindowSize;
	int m_iTime = 0;
	//Noise Map
	GLuint m_iNoiseMapProgram;
	GLuint m_iNoiseMapTime_Location;
	FBOProperty m_NoiseMapFBO;

	vec4 m_v4Plane;


	//Reflect
	FBOProperty m_ReflectFBO;
	float m_fFlipPitch;
	//Refraction
	FBOProperty m_RefractionFBO;
	//DUDV
	GLuint m_iDUDVTextureID;
	GLuint m_iMoveFactor_Location;
	float m_fMoveFactor = 0;
	float m_fMoveSpeed = 0.03f * 0.01f;
	float m_fMoveDirection = 1;
	GLuint m_iPlane_Location;
	//Fresnel
	vec3 m_v3ViewingDirection;
	GLuint m_iViewingDirection_Location;
	//Normal Map
	GLuint m_iNormalMapTexture;
	GLuint m_iHeightMapPixelSize_Location;
	vec2 m_v2HeightMapPixelSize;

	//Simple Plane
	GLuint m_iSimplePlaneProgram;
	GLuint m_iSimplePlaneMVP_Location;
	GLuint m_iSimplePlaneTerrainSize_Location;

	//Shadowing
	GLuint m_iShadowMapTextureID;
	GLuint m_iShadowMatrix_Location;
	mat4 m_ShadowMatrix;
	GLuint m_iLightPosition_Location;
	vec3 m_v3LightPosition;

	mat4 m_InverseModelMatrix;

	//
	vector<GameObject*> m_ListNonCollidedGameObject;
	vector<GameObject*> m_ListCollidedGameObject;
};


#endif // !MESH_H
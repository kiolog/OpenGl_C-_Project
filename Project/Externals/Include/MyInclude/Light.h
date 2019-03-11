#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include"../Include.h"
#include<vector>
#include<string>

using namespace std;
using namespace glm;

class Light {
public:
	Light(vec3 _v3Position,vec3 _v3LookPosition) {
		m_v3Position = _v3Position;
		m_ProjMatrix = ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.f, 100000.f);
		m_v3LookPosition = _v3LookPosition;
		m_ViewMatrix = lookAt(vec3(150,80,150), vec3(0, 0, 0),vec3(0.0f, 1.0f, 0.0f));
		//m_ViewProjMatrix = m_ProjMatrix * m_ViewMatrix;
		float viewportAspect = (float)m_v2DepthTextureSize .x/ (float)m_v2DepthTextureSize.y;
		//m_ViewMatrix = translate(mat4(), vec3(2000, 0, 0));
		//m_ProjMatrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100000.0f);
		m_ViewProjMatrix = m_ProjMatrix * m_ViewMatrix;

		//Set Shader.
		m_iProgram = CreateProgram("../Shader/BaseVertex.shader", "../Shader/BaseFragment.shader");
		//printf("Shader : %d\n", m_iProgram);
		glUseProgram(m_iProgram);
		m_imvp_location = glGetUniformLocation(m_iProgram, "mvp_matrix");
		printf("Light location : %d\n", m_imvp_location);
	}
	mat4 GetViewMatrix() {
		return m_ViewMatrix;
	}
	mat4 GetProjMatrix() {
		return m_ProjMatrix;
	}
	
	vec3 GetLightPosition() {
		return m_v3Position;
	}
	mat4 GetViewProjMatrix() {
		return m_ViewProjMatrix;
	}
	GLuint GetFBO() {
		return m_iDepthFBO;
	}
	vec2 GetDepthSize() {
		return m_v2DepthTextureSize;
	}
	GLuint GetDepthTexture() {
		return m_iDepthTexture;
	}
	void ShadowMapSetting() {
		glUseProgram(m_iProgram);
		glViewport(0, 0, m_v2DepthTextureSize.x, m_v2DepthTextureSize.y);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(4.0f, 4.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, m_iDepthFBO);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static const GLfloat green[] = { 0.f, 1.f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}
	GLuint GetMVPLocation() {
		return m_imvp_location;
	}
	void BindShadowBuffer() {
		printf("Bind Shadow Buffer\n");
		glGenFramebuffers(1, &m_iDepthFBO); //Create FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_iDepthFBO);
		glGenTextures(1, &m_iDepthTexture); //Create fboDataTexture
		glBindTexture(GL_TEXTURE_2D, m_iDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			m_v2DepthTextureSize.x, m_v2DepthTextureSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iDepthTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		printf("Frame Buffer : %d\n", m_iDepthFBO);
		printf("Texture : %d\n", m_iDepthTexture);
	}
private:
	GLuint m_iProgram;
	GLuint m_imvp_location;
	vec3 m_v3Position;
	mat4 m_ProjMatrix;
	mat4 m_ViewMatrix;
	vec3 m_v3LookPosition;
	GLuint m_iDepthFBO;
	GLuint m_iDepthTexture;
	vec2 m_v2DepthTextureSize = vec2(4096, 4096);
	mat4 m_ViewProjMatrix;
};

#endif
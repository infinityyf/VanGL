#pragma once
#ifndef SHADOW
#define SHADOW

#include "frame.h"
#include "shader.h"
class ShadowMap {
public:
	unsigned int shadowFBO;
	unsigned int shadowWidth, shadowHeight;
	unsigned int depthTexture;
	int originWidth;
	int originHeight;
	ShadowMap(glm::vec3 lightPos, glm::vec3 lightDir);

public:
	//std::vector<glm::vec3> lightDirs;
	//we will use light class later
	glm::vec3 lightPos;			//light position
	glm::vec3 lightDir;			//light direction

	glm::mat4 lightSpace;
	StandardShader* depthShader;
	void bindBuffer(int width,int height) {
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		originWidth = width;
		originHeight = height;
		//cull front face
		glCullFace(GL_FRONT);
	}
	void unBindBuffer() {
		//change back to cull back face
		glCullFace(GL_BACK);
		glViewport(0, 0,originWidth, originHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

ShadowMap::ShadowMap(glm::vec3 lightPos, glm::vec3 lightDir) {
	shadowWidth = 1024;
	shadowHeight = 1024;
	glGenFramebuffers(1, &shadowFBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	//set border color
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//set texture as framebuffer's depth buffer(not render buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//can set color/depth/stencil
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//dont need renderbuffer?()
	glDrawBuffer(GL_NONE);			//tell opengl donnt need a color buffer
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	depthShader = new StandardShader("E:\\vs_workspace\\VanGL\\src\\shaders\\shadowMap\\simpleDepthShader.vs","E:\\vs_workspace\\VanGL\\src\\shaders\\shadowMap\\simpleDepthShader.fs");
	
	//set light camera
	float near = 0.01f;
	float far = 10.0f;
	this->lightPos = lightPos;
	this->lightDir = lightDir;
	glm::mat4 orthoProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near, far);
	glm::mat4 lightView = glm::lookAt(lightPos, lightPos+lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpace = orthoProjection * lightView;
	depthShader->use();
	depthShader->setMatrix4("lightMatrix", lightSpace);
}


#endif
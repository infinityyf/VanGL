#pragma once
#ifndef SHADOW
#define SHADOW

#include "frame.h"

class ShadowMap {
public:
	unsigned int shadowFBO;
	unsigned int shadowWidth, shadowHeight;
	unsigned int depthTexture;
	ShadowMap();

	void renderToTexture();
};

ShadowMap::ShadowMap() {
	shadowWidth = 1024;
	shadowHeight = 1024;
	glGenFramebuffers(1, &shadowFBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//set texture as framebuffer's depth buffer(not render buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//can set color/depth/stencil
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//dont need renderbuffer?()
	glDrawBuffer(GL_NONE);			//tell opengl donnt need a color buffer
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

//independent of main camera
void ShadowMap::renderToTexture() {
	glViewport(0, 0, shadowWidth, shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

#endif
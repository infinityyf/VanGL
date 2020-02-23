#pragma once
#ifndef FRAME
#define FRAME
#include <iostream>
#include <glad/glad.h>

float ScreenQuad[] = {
	// NDC coord
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

class Frame {
public:
	unsigned int FBO;
	unsigned int texAttach;
	unsigned int renderBufferObject;
	Frame(int width, int height);
	void use();
};

Frame::Frame(int width, int height) {
	glGenFramebuffers(1, &FBO);
	// also can be binded to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create attachment
	//1. texture attachment
	glGenTextures(1, &texAttach);
	glBindTexture(GL_TEXTURE_2D, texAttach);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach to frame(32) //frame target   //color attach num    //attachment type         //mipmap
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttach, 0);
	//2. render buffer
	glGenRenderbuffers(1, &renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

	// framebuffer set up success
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// now rendering result cannot be blited to screen (need to activate default buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Frame::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}


#endif
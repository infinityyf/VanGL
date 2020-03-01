#pragma once
#ifndef FRAME
#define FRAME
#include <iostream>
#include <glad/glad.h>
#include "shader.h"

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
	

	Frame(int width, int height,bool enableHDR);
	//support MSAA
	Frame(int width, int height,int msaa,bool enableHDR);

	void use();
};

Frame::Frame(int width, int height, bool enableHDR) {
	glGenFramebuffers(1, &FBO);
	// also can be binded to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create attachment
	//1. texture attachment
	glGenTextures(1, &texAttach);
	glBindTexture(GL_TEXTURE_2D, texAttach);
	if (enableHDR) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB16F, GL_FLOAT, NULL);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	
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

//support mass
// glTexImage2DMultisample
// glRenderbufferStorageMultisample
inline Frame::Frame(int width, int height, int msaa, bool enableHDR)
{
	glGenFramebuffers(1, &FBO);
	// also can be binded to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create attachment
	//1. texture attachment,use MSAA
	glGenTextures(1, &texAttach);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texAttach);
	if (enableHDR) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, GL_RGB16F, width, height, GL_TRUE);
	}
	else {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, GL_RGB, width, height, GL_TRUE);
	}
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, GL_RGB, width, height,GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	
	
	//attach to frame(32) //frame target   //color attach num    //attachment type                     //mipmap
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texAttach, 0);
	//2. render buffer
	glGenRenderbuffers(1, &renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa,GL_DEPTH24_STENCIL8, width, height);
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

class Screen {
public:
	unsigned int SVAO, SVBO;
	Screen();
	void Draw(StandardShader* shader,unsigned int screenTexture);
};

inline Screen::Screen()
{
	glGenBuffers(1, &SVBO);
	glGenVertexArrays(1, &SVAO);
	glBindVertexArray(SVAO);
	glBindBuffer(GL_ARRAY_BUFFER, SVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuad), ScreenQuad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);
}

inline void Screen::Draw(StandardShader* shader,unsigned int screenTexture)
{
	shader->use();
	glBindVertexArray(SVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}



#endif
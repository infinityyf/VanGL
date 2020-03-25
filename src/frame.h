#pragma once
#ifndef FRAME
#define FRAME
#include <iostream>
#include <glad/glad.h>
#include "shader.h"

#define MAC_COLOR_ATTACHMENT 8
//this device have 8 color attachment
GLuint attachments[MAC_COLOR_ATTACHMENT] = { 
	GL_COLOR_ATTACHMENT0,	//COLOR_TEXTURE
	GL_COLOR_ATTACHMENT1,	//BLOOM_TEXTURE
	GL_COLOR_ATTACHMENT2,	//POSITION_TEXTURE 
	GL_COLOR_ATTACHMENT3,	//NORMAL_TEXTURE
	GL_COLOR_ATTACHMENT4,	//AMBIENT_TEXTURE
	GL_COLOR_ATTACHMENT5,	//SPECULAR_TEXUTRE 
	GL_COLOR_ATTACHMENT6,	//DIFFUSE_TEXTURE
	GL_COLOR_ATTACHMENT7};

//record what color attachment means
enum COLOR_ATTACH {
	COLOR_TEXTURE = 0,
	BLOOM_TEXTURE = 1,
	POSITION_TEXTURE = 2,
	NORMAL_TEXTURE = 3,
	AMBIENT_TEXTURE = 4,
	SPECULAR_TEXTURE = 5,
	DIFFUSE_TEXTURE = 6,
};

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
	unsigned int texAttachs[MAC_COLOR_ATTACHMENT];
	

	Frame(int width, int height);
	//support MSAA
	Frame(int width, int height,int msaa,bool enableHDR);

	void use();
};

Frame::Frame(int width, int height) {
	glGenTextures(8, texAttachs);
	glGenFramebuffers(1, &FBO);
	// also can be binded to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create attachment
	//1. texture attachment
	
	for (int i = 0; i < MAC_COLOR_ATTACHMENT; i++) {
		glBindTexture(GL_TEXTURE_2D, texAttachs[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		//attach to frame(32) //frame target   //color attach num    //attachment type         //mipmap
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texAttachs[i], 0);
		std::cout << "bind color buffer:" << texAttachs[i] << std::endl;
	}
	glDrawBuffers(8, attachments); //it tells gl which color attachments can be draw on

	//glGenTextures(1, &texAttach);
	//glBindTexture(GL_TEXTURE_2D, texAttach);
	//if (enableHDR) {
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB16F, GL_FLOAT, NULL);
	//}
	//else {
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//}
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);
	////attach to frame(32) //frame target   //color attach num    //attachment type         //mipmap
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttach, 0);
	//2. render buffer
	glGenRenderbuffers(1, &renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
	std::cout << "bind render buffer:" << renderBufferObject << std::endl;
	// framebuffer set up success
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"<< fboStatus << std::endl;
		
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
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, GL_RGBA16F, width, height, GL_TRUE);
	}
	else {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, GL_RGBA, width, height, GL_TRUE);
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

	//combine defferent MRT bass
	void DeferredRender(StandardShader* shader, unsigned int screenTexture[MAC_COLOR_ATTACHMENT]);

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

inline void Screen::DeferredRender(StandardShader* shader, unsigned int screenTexture[MAC_COLOR_ATTACHMENT])
{
	shader->use();
	glBindVertexArray(SVAO);
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture[COLOR_TEXTURE]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, screenTexture[BLOOM_TEXTURE]);*/
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, screenTexture[POSITION_TEXTURE]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, screenTexture[NORMAL_TEXTURE]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, screenTexture[AMBIENT_TEXTURE]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, screenTexture[SPECULAR_TEXTURE]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, screenTexture[DIFFUSE_TEXTURE]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}



#endif
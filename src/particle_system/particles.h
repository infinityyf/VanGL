#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdlib.h>
#include <glad/glad.h>

#include "../shader.h"

struct Particle {
	float position[3];
	float vilocity[3];
	float mass;
	float force[3];
};

class Particles {
public:
	Particle* particleList;
	float* drawBuffer;
	int particleNum;
	float time;

	Particles(int num);
	~Particles();

	void uploadToODE();
	void downloadFromODE();

	GLuint PVBO,PVAO;
	void drawPaticles(StandardShader* shader);

};

Particles::Particles(int num) {
	particleNum = num;
	particleList = (Particle*)malloc(sizeof(Particle)*num);
	drawBuffer = (float*)malloc(sizeof(float) * num * 3);
	time = 0.0f;
	for (int i = 0; i < num; i++) {
		particleList[i] = Particle();
		particleList[i].position[0] = 0.0f;
		particleList[i].position[1] = 0.0f + i/(float)num;
		particleList[i].position[2] = 0.0f;
		particleList[i].vilocity[0] = 0.0f;
		particleList[i].vilocity[1] = 0.0f;
		particleList[i].vilocity[2] = 0.0f;
		particleList[i].mass = 1.0f;
	}
	glGenVertexArrays(1, &PVAO);
	glGenBuffers(1, &PVBO);
	glBindVertexArray(PVAO);
	glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleNum*3, drawBuffer, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3, (void*)0);
	glBindVertexArray(0);

}

Particles::~Particles() {
	free(particleList);
}

inline void Particles::uploadToODE()
{


}

inline void Particles::downloadFromODE()
{
}

inline void Particles::drawPaticles(StandardShader* shader)
{
	for (int i = 0; i < particleNum; i++) {
		drawBuffer[i * 3] = particleList[i].position[0];
		drawBuffer[i * 3 + 1] = particleList[i].position[1];
		drawBuffer[i * 3 + 2] = particleList[i].position[2];
	}
	shader->use();
	glBindVertexArray(PVAO);
	glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleNum * 3, drawBuffer, GL_DYNAMIC_DRAW);

	glDrawArrays(GL_POINTS, 0, particleNum);
	glBindVertexArray(0);
}


#endif
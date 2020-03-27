#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H
#include <random>
#include <vector>
#include <stdlib.h>
#include <glad/glad.h>

#include "../shader.h"
#include "ODE.h"

class Particles {
public:
	std::vector<Particle> particleList;
	std::vector<float> drawBuffer;
	int particleNum;
	float time;

	Particles(int num);
	void uploadToODE();
	void downloadFromODE();
	GLuint PVBO,PVAO;
	void drawPaticles(StandardShader* shader);

};

Particles::Particles(int num) {
	particleNum = num;
	particleList = std::vector<Particle>(num);
	drawBuffer = std::vector<float>(num * 3);
	time = 0.0f;
	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);	//generate random distribution
	std::default_random_engine generator;	//generate engine
	for (int i = 0; i < num; i++) {
		particleList[i].position[0] = randomFloats(generator) * 2.0 - 1.0;
		particleList[i].position[1] = randomFloats(generator) * 2.0 + 1.0;
		particleList[i].position[2] = randomFloats(generator) * 2.0 - 1.0;
		particleList[i].velocity[0] = (randomFloats(generator) * 2.0 - 1.0)/10;
		particleList[i].velocity[1] = (randomFloats(generator) * 2.0 - 1.0)/10;
		particleList[i].velocity[2] = (randomFloats(generator) * 2.0 - 1.0)/10;
		particleList[i].force[0] = 0.0f;
		particleList[i].force[1] = -0.1f;
		particleList[i].force[2] = 0.0f;
		particleList[i].mass = 1.0f;
	}
	glGenVertexArrays(1, &PVAO);
	glGenBuffers(1, &PVBO);
	glBindVertexArray(PVAO);
	glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleNum*3, &drawBuffer[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
	glBindVertexArray(0);

}

inline void Particles::uploadToODE()
{
	ODE::ode(particleList, 0.1f);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleNum * 3, &drawBuffer[0], GL_DYNAMIC_DRAW);

	glDrawArrays(GL_POINTS, 0, particleNum);
	glBindVertexArray(0);
}


#endif
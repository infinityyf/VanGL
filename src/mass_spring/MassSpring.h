#pragma once
#ifndef MASS_SPRING
#define MASS_SPRING
#include <set>
#include <vector>
#include <algorithm>
#include <glad\glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "../load_tet/Tet.h"

struct Particle {	//�ʵ�
	float x;
	float y;
	float z;
	float mass;
};

struct Velocity {	//�ʵ��ٶ�
	float x;
	float y;
	float z;
	float damp;
};

struct Force {		//�ʵ�����
	float x;
	float y;
	float z;
	int linkNum;	//����������
};

struct externalForce {		//����
	float x;
	float y;
	float z;
	float w;
};

struct Spring {		//����
	unsigned int nodeIndex0;
	unsigned int nodeIndex1;
	float stiff;
	float originLength;
};
float modelScale = 6.0;

Particle*	massParticles;		//�ʵ�
Spring*		linkSprings;		//����
Velocity*	particleVelocity;	//�ʵ��ٶ�
Force*		particleFroce;		//�ʵ�����
externalForce* particleExternalFroce;//�ʵ�����
externalForce* implicitFroce;		
int			particleNum;		//�ʵ���
int			tetNum;				//��������
int			linkNum;			//������
std::set<std::pair<unsigned int, unsigned int>> linkEdges;	//��¼����������ȥ���ظ�
std::vector<unsigned int>  particleIndices;			//������
std::vector<unsigned int>  springIndices;			//������

unsigned int DVBO;				//��������Ҫ�ı���
unsigned int DMEBO;
unsigned int DSEBO;
unsigned int DVAO;
enum DRAW_MODE					//���Ʒ�ʽ
{
	SPRING = 0,
	MASS = 1,
	EXPLICIT = 2,
	IMPLICIT = 3,
};

void InitializeSystem(TETmodel* mesh) {
	if (massParticles) free(massParticles);
	if (linkSprings) free(linkSprings);
	if (particleVelocity) free(particleVelocity);
	if (particleFroce) free(particleFroce);
	if (particleExternalFroce) free(particleExternalFroce);
	if (implicitFroce) free(implicitFroce);
	//��¼����
	for (int v = 0; v < mesh->numtetrahedra; v++) {
		// store each edge of the tetrahedron as a pair of indices
		for (int i = 0; i < 4; ++i) {
			int v0 = mesh->elements[v].pindices[i];
			for (int j = i + 1; j < 4; ++j) {
				int v1 = mesh->elements[v].pindices[j];
				linkEdges.insert(std::pair<unsigned int, unsigned int>(std::min(v0, v1), std::max(v0, v1)));
			}
		}
	}

	particleNum = mesh->numvertices;
	tetNum = mesh->numtetrahedra;
	linkNum = linkEdges.size();
	massParticles = (Particle*)malloc(sizeof(Particle)*particleNum);
	particleVelocity = (Velocity*)malloc(sizeof(Velocity)*particleNum);
	particleFroce = (Force*)malloc(sizeof(Force)*particleNum);
	linkSprings = (Spring*)malloc(sizeof(Spring)*linkNum);
	particleExternalFroce = (externalForce*)malloc(sizeof(externalForce)*particleNum);
	implicitFroce = (externalForce*)malloc(sizeof(externalForce)*particleNum);
	//���ƶ���
	for (int i = 0; i < mesh->numvertices; i++) {
		massParticles[i].x = mesh->vertices[i].x * modelScale;	//��ʼ��λ�ú�����
		massParticles[i].y = mesh->vertices[i].y * modelScale;
		massParticles[i].z = mesh->vertices[i].z * modelScale;
		massParticles[i].mass = 0.1;	//KG
		particleVelocity[i].x = 0.0;							//��ʼ���ٶ�
		particleVelocity[i].y = 0.0;
		particleVelocity[i].z = 0.0;
		particleVelocity[i].damp = 50.0;
		particleFroce[i].x = 0.0;								//��ʼ���ڲ�����
		particleFroce[i].y = 0.0;
		particleFroce[i].z = 0.0;
		particleFroce[i].linkNum = 0;
		implicitFroce[i].x = 0.0;								//��ʼ����ʽ��������
		implicitFroce[i].y = 0.0;
		implicitFroce[i].z = 0.0;
		particleExternalFroce[i].x = 0.0;						//��ʼ������
		particleExternalFroce[i].y = -10;
		particleExternalFroce[i].z = 0.0;
		particleIndices.push_back(i);
	}
	//���ɵ���
	std::set<std::pair<unsigned int, unsigned int>>::iterator i;	//��ȡ��������������
	int springIndex = 0;
	for (i = linkEdges.begin(); i != linkEdges.end(); i++) {
		float deltaX = massParticles[i->first].x - massParticles[i->second].x;
		float deltaY = massParticles[i->first].y - massParticles[i->second].y;
		float deltaZ = massParticles[i->first].z - massParticles[i->second].z;
		linkSprings[springIndex].nodeIndex0 = i->first;
		linkSprings[springIndex].nodeIndex1 = i->second;
		linkSprings[springIndex].stiff = 50000.0;	// N/m
		linkSprings[springIndex].originLength = std::sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ* deltaZ);
		springIndices.push_back(i->first);
		springIndices.push_back(i->second);
		particleFroce[i->first].linkNum++;			//���ڵ�����������
		particleFroce[i->second].linkNum++;
		springIndex++;
	}


	//set for rendering
	glGenVertexArrays(1, &DVAO);
	glGenBuffers(1, &DVBO);
	glGenBuffers(1, &DMEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DMEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, particleIndices.size() * sizeof(unsigned int), &particleIndices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &DSEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DSEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, springIndices.size() * sizeof(unsigned int), &springIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(DVAO);
	glBindBuffer(GL_ARRAY_BUFFER, DVBO);
	glBufferData(GL_ARRAY_BUFFER, particleNum * sizeof(Particle), massParticles, GL_DYNAMIC_DRAW);
	
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
	glBindVertexArray(0);
}

void updateSystem(float timeInterval,int updateModel) {
	if (updateModel == EXPLICIT) {
		//explicit euler
		for (int i = 0; i < particleNum; i++) {
			particleFroce[i].x = 0.0;
			particleFroce[i].y = 0.0;
			particleFroce[i].z = 0.0;
		}
		for (int i = 0; i < linkNum; i++) {
			unsigned int particle0Index = linkSprings[i].nodeIndex0;
			unsigned int particle1Index = linkSprings[i].nodeIndex1;
			float deltaX = massParticles[particle0Index].x - massParticles[particle1Index].x;
			float deltaY = massParticles[particle0Index].y - massParticles[particle1Index].y;
			float deltaZ = massParticles[particle0Index].z - massParticles[particle1Index].z;
			float distance = std::sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ* deltaZ);
			float force = linkSprings[i].stiff * (distance - linkSprings[i].originLength);
			glm::vec3 forceDirection(deltaX, deltaY, deltaZ);
			glm::vec3 currentForce = glm::normalize(forceDirection) * force;
			particleFroce[particle0Index].x += -currentForce.x;
			particleFroce[particle0Index].y += -currentForce.y;
			particleFroce[particle0Index].z += -currentForce.z;
			particleFroce[particle1Index].x += currentForce.x;
			particleFroce[particle1Index].y += currentForce.y;
			particleFroce[particle1Index].z += currentForce.z;
		}
		for (int i = 0; i < particleNum; i++) {
			if (massParticles[i].y < 0.0) {
				particleVelocity[i].x *= -0.0;
				particleVelocity[i].y *= -0.0;
				particleVelocity[i].z *= -0.0;
				particleExternalFroce[i].x = 0.0;
				particleExternalFroce[i].y = 0.0;
				particleExternalFroce[i].z = 0.0;
			}
			glm::vec3 dampForce = -glm::vec3(particleVelocity[i].x, particleVelocity[i].y, particleVelocity[i].z)*particleVelocity[i].damp;
			glm::vec3 externalfroce = glm::vec3(particleExternalFroce[i].x, particleExternalFroce[i].y, particleExternalFroce[i].z);
			glm::vec3 force = glm::vec3(particleFroce[i].x, particleFroce[i].y, particleFroce[i].z) + dampForce + externalfroce;
			glm::vec3 acceleration = force / massParticles[i].mass;
			particleVelocity[i].x += acceleration.x * timeInterval;
			particleVelocity[i].y += acceleration.y * timeInterval;
			particleVelocity[i].z += acceleration.z * timeInterval;
			massParticles[i].x += particleVelocity[i].x * timeInterval;
			massParticles[i].y += particleVelocity[i].y * timeInterval;
			massParticles[i].z += particleVelocity[i].z * timeInterval;
		}
	}
	if (updateModel == IMPLICIT) {
		//implicit euler
		for (int i = 0; i < particleNum; i++) {
			particleFroce[i].x = 0.0;
			particleFroce[i].y = 0.0;
			particleFroce[i].z = 0.0;
			implicitFroce[i].x = 0.0;
			implicitFroce[i].y = 0.0;
			implicitFroce[i].z = 0.0;
		}
		for (int i = 0; i < linkNum; i++) {
			unsigned int particle0Index = linkSprings[i].nodeIndex0;
			unsigned int particle1Index = linkSprings[i].nodeIndex1;
			float deltaX = massParticles[particle0Index].x - massParticles[particle1Index].x;
			float deltaY = massParticles[particle0Index].y - massParticles[particle1Index].y;
			float deltaZ = massParticles[particle0Index].z - massParticles[particle1Index].z;
			float distance = std::sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ* deltaZ);
			float force = linkSprings[i].stiff * (distance - linkSprings[i].originLength);
			glm::vec3 forceDirection(deltaX, deltaY, deltaZ);
			glm::vec3 currentForce = glm::normalize(forceDirection) * force;
			particleFroce[particle0Index].x += -currentForce.x;				//������������
			particleFroce[particle0Index].y += -currentForce.y;
			particleFroce[particle0Index].z += -currentForce.z;
			particleFroce[particle1Index].x += currentForce.x;
			particleFroce[particle1Index].y += currentForce.y;
			particleFroce[particle1Index].z += currentForce.z;
		}
		//apply external force
		for (int i = 0; i < particleNum; i++) {
			particleFroce[i].x += (particleExternalFroce[i].x - 0.1*particleVelocity[i].x);				//Ӧ������,����
			particleFroce[i].y += (particleExternalFroce[i].y - 0.1*particleVelocity[i].y);
			particleFroce[i].z += (particleExternalFroce[i].z - 0.1*particleVelocity[i].z);
		}
		for (int i = 0; i < linkNum; i++) {
			unsigned int particle0Index = linkSprings[i].nodeIndex0;
			unsigned int particle1Index = linkSprings[i].nodeIndex1;

			//������ʽ����ʹ�õ���
			implicitFroce[particle0Index].x += linkSprings[i].stiff*particleFroce[particle1Index].x * timeInterval / (massParticles[particle1Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle1Index].linkNum);
			implicitFroce[particle0Index].y += linkSprings[i].stiff*particleFroce[particle1Index].y * timeInterval / (massParticles[particle1Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle1Index].linkNum);
			implicitFroce[particle0Index].z += linkSprings[i].stiff*particleFroce[particle1Index].z * timeInterval / (massParticles[particle1Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle1Index].linkNum);
			implicitFroce[particle1Index].x += linkSprings[i].stiff*particleFroce[particle0Index].x * timeInterval / (massParticles[particle0Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle0Index].linkNum);
			implicitFroce[particle1Index].y += linkSprings[i].stiff*particleFroce[particle0Index].y * timeInterval / (massParticles[particle0Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle0Index].linkNum);
			implicitFroce[particle1Index].z += linkSprings[i].stiff*particleFroce[particle0Index].z * timeInterval / (massParticles[particle0Index].mass + timeInterval*timeInterval*linkSprings[i].stiff*particleFroce[particle0Index].linkNum);
		}
		for (int i = 0; i < particleNum; i++) {
			if (massParticles[i].y < 0.0) {
				particleVelocity[i].x *= -0.0;
				particleVelocity[i].y *= -0.0;
				particleVelocity[i].z *= -0.0;
				particleExternalFroce[i].x = 0.0;
				particleExternalFroce[i].y = 0.0;
				particleExternalFroce[i].z = 0.0;
			}
			particleVelocity[i].x += (particleFroce[i].x * timeInterval + timeInterval*timeInterval*implicitFroce[i].x) / (massParticles[i].mass + timeInterval*timeInterval * 100000.0 * particleFroce[i].linkNum);
			particleVelocity[i].y += (particleFroce[i].y * timeInterval + timeInterval*timeInterval*implicitFroce[i].y) / (massParticles[i].mass + timeInterval*timeInterval * 100000.0 * particleFroce[i].linkNum);
			particleVelocity[i].z += (particleFroce[i].z * timeInterval + timeInterval*timeInterval*implicitFroce[i].z) / (massParticles[i].mass + timeInterval*timeInterval * 100000.0 * particleFroce[i].linkNum);
			massParticles[i].x += particleVelocity[i].x * timeInterval;		//ͨ��n+1ʱ�̵��ٶ�����õ�
			massParticles[i].y += particleVelocity[i].y * timeInterval;
			massParticles[i].z += particleVelocity[i].z * timeInterval;
		}
	}

}

void drawMassSpring(int mode) {
	switch (mode)
	{
	case SPRING: {
		glBindVertexArray(DVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DSEBO);
		glBindBuffer(GL_ARRAY_BUFFER, DVBO);
		glBufferData(GL_ARRAY_BUFFER, particleNum * sizeof(Particle), massParticles, GL_DYNAMIC_DRAW);
		glDrawElements(GL_LINES, springIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		break;
	}
	case MASS: {
		glBindVertexArray(DVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DMEBO);
		glBindBuffer(GL_ARRAY_BUFFER, DVBO);
		glBufferData(GL_ARRAY_BUFFER, particleNum * sizeof(Particle), massParticles, GL_DYNAMIC_DRAW);
		glDrawElements(GL_POINTS, particleIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		break;
	}
	default:
		break;
	}
}

#endif

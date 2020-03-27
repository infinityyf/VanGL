#pragma once
#ifndef ODE_H
#define ODE_H

#include <vector>


struct Particle {
	float position[3];
	float velocity[3];
	float mass;
	float force[3];
};

struct Force {
	float force[3];
};

class ODE {
public:
	//x(t+1) = x(t) + hv(t)
	//v(t+1) = v(t) + hf(t)/m
	static void ode(std::vector<Particle>& particleList,float delatTime);
};

inline void ODE::ode(std::vector<Particle>& particleList, float delatTime) {
	//update velocity
	for (int particleIndex = 0; particleIndex < particleList.size(); particleIndex++) {
		particleList[particleIndex].position[0] += particleList[particleIndex].velocity[0] * delatTime;
		particleList[particleIndex].position[1] += particleList[particleIndex].velocity[1] * delatTime;
		particleList[particleIndex].position[2] += particleList[particleIndex].velocity[2] * delatTime;
		particleList[particleIndex].velocity[0] += (particleList[particleIndex].force[0]/ particleList[particleIndex].mass)*delatTime;
		particleList[particleIndex].velocity[1] += (particleList[particleIndex].force[1]/ particleList[particleIndex].mass)*delatTime;
		particleList[particleIndex].velocity[2] += (particleList[particleIndex].force[2]/ particleList[particleIndex].mass)*delatTime;
	}
}

#endif // !ODE_H

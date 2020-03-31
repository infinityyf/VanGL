#pragma once
#ifndef SPRING_LINK
#define SPRING_LINK
#include "MassParticle.h"

class SpringLink {
public:
	MassParticle* m_node0;			//node of spring
	MassParticle* m_node1;			//node of spring
	double		m_springElongation;
	bool		m_enable;
	double		m_length;			//init length

public:
	SpringLink(MassParticle* node0, MassParticle* node1);
	void computeForce();
};

SpringLink::SpringLink(MassParticle* node0, MassParticle* node1) {
	m_node0 = node0;
	m_node1 = node1;
	m_enable = true;
	m_length = glm::distance(node0->m_position, node1->m_position);
	m_springElongation = 100.0;
}
//calculate force of two particles
inline void SpringLink::computeForce()
{
	// current distance
	vec3d link01 = m_node1->m_position - m_node0->m_position;
	double distance = glm::length(link01);

	// if distance too small, no forces are applied
	if (distance < 0.000001) { return; }

	// elongation compute force
	double f = m_springElongation * (distance - m_length);

	// apply force
	if (distance > 0.000001)
	{
		vec3d force = link01/ distance * f;
		m_node0->addForce(force);
		vec3d tmpfrc = -force;
		m_node1->addForce(tmpfrc);
	}
}

#endif

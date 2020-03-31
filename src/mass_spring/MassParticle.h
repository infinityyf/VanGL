#pragma once
#ifndef MASS_PARTICLE
#define MASS_PARTICLE

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using vec3d = glm::dvec3;
using vec3f = glm::vec3;



class MassParticle {
public:
	vec3d	m_position;					//current position
	vec3d	m_nextPosition;				//next position
	double	m_mass;						//massive
	vec3d	m_force;					//current iner force
	vec3d	m_acceleration;				//current acc
	vec3d	m_velocity;					//current vel
	double	m_damping;					//moving damping
	bool	m_fixed;					//isfixed?
	vec3d	m_externalForce;			//current external Force

public:
	MassParticle();
	MassParticle(vec3d position);
	void addForce(vec3d& force);
	void setExternalForce(vec3d& a_force);
	void computeNextPose(double a_timeInterval);
	void clearForces();
	void clearExternalForces();
	void applyNextPos();
	void render();
};
MassParticle::MassParticle() {
	m_position = vec3d(0.0);
	m_externalForce = vec3d(0.0);
	m_force = vec3d(0.0);
	m_acceleration = vec3d(0.0);
	m_velocity = vec3d(0.0);
	m_damping = 5.0;
	m_fixed = false;
	m_mass = 0.1;
}

inline MassParticle::MassParticle(vec3d position)
{
	m_position = position;
	m_externalForce = vec3d(0.0);
	m_force = vec3d(0.0);
	m_acceleration = vec3d(0.0);
	m_velocity = vec3d(0.0);
	m_damping = 5.0;
	m_fixed = false;
	m_mass = 0.1;
}

//add inter force to current particle
inline void MassParticle::addForce(vec3d& force) {
	m_force += force;
}
//set extern force to current particle
inline void MassParticle::setExternalForce(vec3d& force)
{
	m_externalForce = force;
}
//compute update position
inline void MassParticle::computeNextPose(double timeInterval)
{
	if (!m_fixed) {
		//euler integration
		vec3d dampForce;
		dampForce = -m_velocity * m_damping;
		m_force += dampForce;
		//m_force = glm::normalize(m_position - vec3d(0.0));
		m_acceleration = (m_force + m_externalForce) / m_mass;
		m_velocity += m_acceleration * timeInterval;
		m_nextPosition = m_position + m_velocity * timeInterval;
	}
	else {
		m_nextPosition = m_position;
	}
}
//clear all inter force
inline void MassParticle::clearForces()
{
	m_force = vec3d(0.0);
}
//clear all extern force
inline void MassParticle::clearExternalForces()
{
	m_externalForce = vec3d(0.0);
}
//update current position
inline void MassParticle::applyNextPos()
{
	m_position = m_nextPosition;
}
inline void MassParticle::render()
{
}
#endif
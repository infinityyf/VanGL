#pragma once
#ifndef DYNAMIC_WORLD
#define DYNAMIC_WORLD
#include <vector>
#include "DynamicMesh.h"

class DynamicWorld {
public:
	double m_integrationTime;
	double m_simulationTime;
	std::vector<DynamicMesh> m_meshes;
	
public:
	DynamicWorld();
	void addDynamicMesh(Mesh* mesh);
	//update dynamic
	void updatePyhsics();
	//update graphic
	void updateGraphic();
	//draw mesh
	void render();
};

inline DynamicWorld::DynamicWorld()
{
	m_simulationTime = 0.0;
	m_integrationTime = 1.0f / 400.0f; 
	m_meshes.clear();
}

inline void DynamicWorld::addDynamicMesh(Mesh* mesh)
{
	DynamicMesh dyMesh(mesh);
	m_meshes.push_back(dyMesh);
}
//update massparticle position
inline void DynamicWorld::updatePyhsics() {
	std::vector<DynamicMesh>::iterator i;

	for (i = m_meshes.begin(); i < m_meshes.end(); i++) {
		(*i).clearForce();
		(*i).clearExternalForce();
	}
	for (i = m_meshes.begin(); i < m_meshes.end(); i++) {
		(*i).computeForces();
	}
	for (i = m_meshes.begin(); i < m_meshes.end(); i++) {
		(*i).computeNextPos(m_integrationTime);
		(*i).updateMesh();
	}
}
//update vertex position for rendering
inline void DynamicWorld::updateGraphic()
{
	for (int i = 0; i < m_meshes.size(); i++) {
		for (int j = 0; j < m_meshes[i].m_particles.size(); j++) {
			m_meshes[i].m_vertexs[j].Position = m_meshes[i].m_particles[j].m_position;
		}
	}
}

inline void DynamicWorld::render()
{
	for (int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i].render();
	}
}

#endif // !DYNAMIC_WORLD

#pragma once
#ifndef DYNAMIC_MESH
#define DYNAMIC_MESH
#include <vector>
#include <glad/glad.h>
#include "MassParticle.h"
#include "SpringLink.h"
#include "../mesh.h"

struct DynamicVertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};

class DynamicMesh {
public:
	//for rendering
	std::vector<DynamicVertex> m_vertexs;
	std::vector<unsigned int> m_indices;
	unsigned int DVBO;
	unsigned int DEBO;
	unsigned int DVAO;
public:
	std::vector<MassParticle> m_particles;
	std::vector<SpringLink> m_springs;
public:
	DynamicMesh(Mesh* mesh);
	void clearForce();
	void clearExternalForce();
	void computeForces();
	void computeNextPos(double timeInterval);
	void updateMesh();
	void render();
};
//build physic model from geometry model
inline DynamicMesh::DynamicMesh(Mesh* mesh)
{
	std::vector<unsigned int>::iterator i;
	std::vector<Vertex>::iterator v;
	m_particles.clear();
	m_springs.clear();
	m_vertexs.clear();
	m_indices = mesh->indices;
	for (v = mesh->vertexs.begin(); v < mesh->vertexs.end(); v++) {
		//add graphic vertex
		DynamicVertex dv;
		dv.Position = (*v).Position;
		dv.TexCoords = (*v).TexCoords;
		m_vertexs.push_back(dv);
		//add physic vertex
		MassParticle mp(dv.Position);
		m_particles.push_back(mp);
	}
	for (i = mesh->indices.begin(); i < mesh->indices.end(); i+=3) {
		
		MassParticle mp0 = m_particles[*i];
		MassParticle mp1 = m_particles[*(i+1)];
		MassParticle mp2 = m_particles[*(i+2)];
		SpringLink sl0(&mp0, &mp1);
		SpringLink sl1(&mp1, &mp2);
		SpringLink sl2(&mp0, &mp2);
		//add springs
		m_springs.push_back(sl0);
		m_springs.push_back(sl1);
		m_springs.push_back(sl2);
	}
	
	//set for rendering
	glGenVertexArrays(1, &DVAO);
	glGenBuffers(1, &DVBO);
	glGenBuffers(1, &DEBO);
	glBindVertexArray(DVAO);
	glBindBuffer(GL_ARRAY_BUFFER, DVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(DynamicVertex), &m_vertexs[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicVertex), (void*)0);
	// tex coord
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertex), (void*)offsetof(DynamicVertex, TexCoords));
	glBindVertexArray(0);
}

inline void DynamicMesh::clearForce()
{
	std::vector<MassParticle>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); ++i)
	{
		i->clearForces();
	}
}
inline void DynamicMesh::clearExternalForce()
{
	std::vector<MassParticle>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); ++i)
	{
		i->clearExternalForces();
	}
}
inline void DynamicMesh::computeForces()
{
	//set interforce to related particles
	std::vector<SpringLink>::iterator i;
	for (i = m_springs.begin(); i != m_springs.end(); ++i)
	{
		i->computeForce();
	}
}
//calculate next positions
inline void DynamicMesh::computeNextPos(double timeInterval)
{
	std::vector<MassParticle>::iterator i;

	for (i = m_particles.begin(); i != m_particles.end(); ++i)
	{
		i->computeNextPose(timeInterval);
	}
}
//update mesh vertexs position
inline void DynamicMesh::updateMesh()
{
	std::vector<MassParticle>::iterator i;

	for (i = m_particles.begin(); i != m_particles.end(); ++i)
	{
		i->applyNextPos();
	}
}
inline void DynamicMesh::render()
{
	glBindVertexArray(DVAO);
	glBindBuffer(GL_ARRAY_BUFFER, DVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(DynamicVertex), &m_vertexs[0], GL_DYNAMIC_DRAW);
	glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
#endif // !DYNAMIC_MESH

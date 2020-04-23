#pragma once
#ifndef STANDARD_MESH
#define STANDARD_MESH

#include <vector>

#include"assimp/config.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "../texture.h"
#include "../material/material.h"

// vertex information
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent; //tangent coord 
};

class StandardMesh {
public:
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	StandardMesh(std::vector<Vertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void setupMesh();
	//draw in traditional way
	void drawMesh();
	void setupMaterial(unsigned int vertex, unsigned int fragment, unsigned int tessellationControl, unsigned int tessellationEvaluation, unsigned int geometry);
public:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;

public:
	ForwardStandardMaterial* material;

};

StandardMesh::StandardMesh(std::vector<Vertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertexs = vertexs;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
	

}

void StandardMesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//then vertex attribute will bind to VAO 
	//just change different VAO when rendering different objects
	glBindVertexArray(VAO);
	// set buffer type and indicate that we will set VBO later
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//GL_STATIC_DRAW : data wont change
	//GL_DYNAMIC_DRAW : data change a lot
	//GL_STREAM_DRAW : data change every time
	glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//how to resolve tha vertex data
	//first 0 means location 0 in vertex shader
	//GL_FALSE means we dont want map data between 0,1 or -1,1
	//this funciton will run above VBO bind to GL_ARRAY_BUFFER
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// tex coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	//tangent vector
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glBindVertexArray(0);

}
inline void StandardMesh::drawMesh()
{
	glActiveTexture(GL_TEXTURE0);
	material->setInt("material.diffuse", 0, material->fragmentProgram);
	glBindTexture(GL_TEXTURE_2D, material->diffuseTexture);
	glActiveTexture(GL_TEXTURE1);
	material->setInt("material.speqular", 1, material->fragmentProgram);
	glBindTexture(GL_TEXTURE_2D, material->specularTexture);
	glActiveTexture(GL_TEXTURE2);
	material->setInt("material.ambient", 2, material->fragmentProgram);
	glBindTexture(GL_TEXTURE_2D, material->ambientTexture);
	glActiveTexture(GL_TEXTURE3);
	material->setInt("material.normal", 3, material->fragmentProgram);
	glBindTexture(GL_TEXTURE_2D, material->normalTexture);

	//assemble the programs
	material->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

inline void StandardMesh::setupMaterial(unsigned int vertex, unsigned int fragment, unsigned int tessellationControl, unsigned int tessellationEvaluation, unsigned int geometry)
{
	material = new ForwardStandardMaterial(vertex, fragment, tessellationControl, tessellationEvaluation, geometry);
	unsigned int i;
	//set textures to material
	for (i = 0; i < textures.size(); i++) {
		unsigned int type = textures[i].textureType;
		switch (type)
		{
		case DIFFUSE_TEX: {
			material->setTexture(textures[i].textureID, DIFFUSE_TEX);
			break; }
		case SPECULAR_TEX: {
			material->setTexture(textures[i].textureID, SPECULAR_TEX);
			break; }
		case AMBIENT_TEX: {
			material->setTexture(textures[i].textureID, AMBIENT_TEX);
			break; }
		case NORMAL_TEX: {
			material->setTexture(textures[i].textureID, NORMAL_TEX);
			break;
		}
		default:
			break;
		}

	}
}

#endif
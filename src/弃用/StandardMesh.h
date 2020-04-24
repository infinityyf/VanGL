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
struct StandardVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent; //tangent coord 
};

class StandardMesh {
public:
	std::vector<StandardVertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	StandardMesh(std::vector<StandardVertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void setupMesh();
	//draw in traditional way
	void drawMesh(ForwardStandardMaterial* material,unsigned int sky,unsigned shadowID);
public:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;

};

StandardMesh::StandardMesh(std::vector<StandardVertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures) {
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
	glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(StandardVertex), &vertexs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//how to resolve tha vertex data
	//first 0 means location 0 in vertex shader
	//GL_FALSE means we dont want map data between 0,1 or -1,1
	//this funciton will run above VBO bind to GL_ARRAY_BUFFER
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StandardVertex), (void*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StandardVertex), (void*)offsetof(StandardVertex, Normal));
	// tex coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StandardVertex), (void*)offsetof(StandardVertex, TexCoords));
	//tangent vector
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(StandardVertex), (void*)offsetof(StandardVertex, Tangent));
	glBindVertexArray(0);

}
inline void StandardMesh::drawMesh(ForwardStandardMaterial* material, unsigned int sky, unsigned shadowID)
{
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		unsigned int type = textures[i].textureType;
		switch (type)
		{
			// set texture1 uniform to texture unit0(bind with GPU texture unit not data)
			//case AMBIENT_TEX: shader->setInt("material.ambient", i);
		case DIFFUSE_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			material->setInt("material.diffuse", i, material->fragmentProgram);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case SPECULAR_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			material->setInt("material.specular", i, material->fragmentProgram);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case AMBIENT_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			material->setInt("material.ambient", i, material->fragmentProgram);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case NORMAL_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			material->setInt("material.normal", i, material->fragmentProgram);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break;
		}
		default:
			break;
		}

	}
	material->setFloat("material.shininess", 32.0f, material->fragmentProgram);
	// load sky box
	glActiveTexture(GL_TEXTURE0 + i);
	material->setInt("material.sky", i, material->fragmentProgram);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
	//load shadow map
	if (shadowID != NULL) {
		glActiveTexture(GL_TEXTURE0 + (i + 1));
		material->setInt("shadowMap", (i + 1), material->fragmentProgram);
		glBindTexture(GL_TEXTURE_2D, shadowID);
	}
	//assemble the programs
	material->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

#endif
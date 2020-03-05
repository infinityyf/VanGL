#pragma once
#ifndef MESH
#define MESH
#include <vector>

#include"assimp/config.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "texture.h"

#include "shader.h"

// vertex information
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh {
public:
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	Mesh(std::vector<Vertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void setupMesh();
	void drawMesh(StandardShader* shader,Skybox* sky, int shadowID=NULL);
	void drawMeshInstanced(StandardShader* shader, Skybox* sky, int amount);
public:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;

};

Mesh::Mesh(std::vector<Vertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertexs = vertexs;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
}

void Mesh::setupMesh() {
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
	glBindVertexArray(0);
	
}

inline void Mesh::drawMesh(StandardShader* shader,Skybox* sky,int shadowID)
{
	shader->use();	
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		
		unsigned int type = textures[i].textureType;
		switch (type)
		{
			// set texture1 uniform to texture unit0(bind with GPU texture unit not data)
			//case AMBIENT_TEX: shader->setInt("material.ambient", i);
		case DIFFUSE_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.diffuse", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case SPECULAR_TEX: { 
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.specular", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break;}
		case AMBIENT_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.ambient", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		//case NORMAL_TEX: shader->setInt("material.normal", i);
		default:
			break;
		}
		
	}
	shader->setFloat("material.shininess", 32.0f);
	// load sky box
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("material.sky", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky->skyBox);
	if (shadowID != NULL) {
		//load shadow map
		glActiveTexture(GL_TEXTURE0 + (i + 1));
		shader->setInt("shadowMap", (i + 1));
		glBindTexture(GL_TEXTURE_2D, shadowID);
	}
	
	glBindVertexArray(VAO);
	// third parameter data type
	// forth parameter offset 
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

inline void Mesh::drawMeshInstanced(StandardShader* shader, Skybox* sky, int amount)
{
	shader->use();
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {

		unsigned int type = textures[i].textureType;
		switch (type)
		{
			// set texture1 uniform to texture unit0(bind with GPU texture unit not data)
			//case AMBIENT_TEX: shader->setInt("material.ambient", i);
		case DIFFUSE_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.diffuse", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case SPECULAR_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.specular", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case AMBIENT_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.ambient", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
						//case NORMAL_TEX: shader->setInt("material.normal", i);
		default:
			break;
		}

	}
	shader->setFloat("material.shininess", 32.0f);
	// load sky box
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("material.sky", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky->skyBox);


	glBindVertexArray(VAO);
	// third parameter data type
	// forth parameter offset 
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,amount);
	glBindVertexArray(0);
}

#endif // ! MESH

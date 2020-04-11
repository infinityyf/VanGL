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
	glm::vec3 Tangent; //tangent coord 
};

class Mesh {
public:
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	Mesh(std::vector<Vertex> vertexs, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void setupMesh();
	//draw in traditional way
	void drawMesh(StandardShader* shader, unsigned int sky, int shadowID=NULL);
	//draw in PBR way
	void drawMeshPBR(StandardShader* shader, unsigned int irradiance, unsigned int prefilter, unsigned int brdfLUT, int shadowID = NULL);
	void drawMeshInstanced(StandardShader* shader, unsigned int sky, int amount);
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
	//tangent vector
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glBindVertexArray(0);
	
}

inline void Mesh::drawMesh(StandardShader* shader, unsigned int sky,int shadowID)
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
		case NORMAL_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.normal", i); 
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break;
		}
		default:
			break;
		}
		
	}
	shader->setFloat("material.shininess", 32.0f);
	// load sky box
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("material.sky", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
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

inline void Mesh::drawMeshPBR(StandardShader* shader, unsigned int irradiance, unsigned int prefilter, unsigned int brdfLUT, int shadowID)
{
	shader->use();
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {

		unsigned int type = textures[i].textureType;
		switch (type)
		{
			// set texture1 uniform to texture unit0(bind with GPU texture unit not data)
			//case AMBIENT_TEX: shader->setInt("material.ambient", i);
		case AMBIENT_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("PBRmaterial.albedoMap", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case NORMAL_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("PBRmaterial.normalMap", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case METALLIC_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("PBRmaterial.metallicMap", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		case ROUGHNESS_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("PBRmaterial.rougnnessMap", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break;}
		case AO_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("PBRmaterial.aoMap", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break; }
		default:
			break;
		}

	}
	// load cube maps
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("irradianceMap", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance);
	i++;
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("prefilterMap", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter);
	i++;
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("brdfMap", i);
	glBindTexture(GL_TEXTURE_2D, brdfLUT);
	i++;
	if (shadowID != NULL) {
		//load shadow map
		glActiveTexture(GL_TEXTURE0 + (i + 1));
		shader->setInt("shadowMap", (i + 1));
		glBindTexture(GL_TEXTURE_2D, shadowID);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

inline void Mesh::drawMeshInstanced(StandardShader* shader, unsigned int sky, int amount)
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
		case NORMAL_TEX: {
			glActiveTexture(GL_TEXTURE0 + i);
			shader->setInt("material.normal", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
			break;
		}
		default:
			break;
		}

	}
	shader->setFloat("material.shininess", 32.0f);
	// load sky box
	glActiveTexture(GL_TEXTURE0 + i);
	shader->setInt("material.sky", i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky);


	glBindVertexArray(VAO);
	// third parameter data type
	// forth parameter offset 
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,amount);
	glBindVertexArray(0);
}

#endif // ! MESH

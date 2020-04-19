#pragma once
#ifndef PLANE
#define PLANE
#include "glad/glad.h"
#include "../shader.h"
#include "../texture.h"

//float planeVertices[] = {
//	// positions            // normals         // texcoords
//	-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
//	-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
//	 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
//
//	 5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
//	-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
//	 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
//	 
//	 5.0f, -0.5f /*- FLT_MIN*/,  5.0f,  0.0f, -1.0f, 0.0f,  1.0f,  0.0f,
//	-5.0f, -0.5f /*- FLT_MIN*/,  5.0f,  0.0f, -1.0f, 0.0f,   0.0f,  0.0f,
//	-5.0f, -0.5f /*- FLT_MIN*/, -5.0f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
//	 							 
//	 5.0f, -0.5f /*- FLT_MIN*/,  5.0f,  0.0f, -1.0f, 0.0f,  1.0f,  0.0f,
//	-5.0f, -0.5f /*- FLT_MIN*/, -5.0f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
//	 5.0f, -0.5f /*- FLT_MIN*/, -5.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
//};
float planeVertices[] = {
	// positions            // normals         // texcoords
	-1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.5f,
	-1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	 0.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 1.0f,

	 0.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 1.0f,
	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,
	-1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.5f,

	 //2
	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,
	 0.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 1.0f,
	 1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

	 1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.5f,
	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,

	 //3
	 0.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.0f,
	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,
	 1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.5f,

	 1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.5f,
	 1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.0f,

	 //4
	-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.5f,
	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,

	 0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,
	 0.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.0f,
	-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
};

class Plane {
public:
	unsigned int VBO, VAO;
	unsigned int textureID;
	unsigned int NormalTextureID;	//normal map(may not use)
	unsigned int HeightTextureID;	//height map(may not use)
	Plane(unsigned int textureID=NULL);
	void Draw(StandardShader* shader,int shadowID=NULL);
	void DrawDebug(StandardShader* shader);

	glm::mat4 model;
};


Plane::Plane(unsigned int textureID) {
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);

	if(!textureID)
		this->textureID = textureID;
	model = glm::mat4(1.0f);
}

void Plane::Draw(StandardShader* shader,int shadowID) {
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, HeightTextureID);
	if (shadowID != NULL) {
		shader->setInt("shadowMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowID);
	}
	shader->setMatrix4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
}

inline void Plane::DrawDebug(StandardShader* shader)
{
	shader->use();
	shader->setMatrix4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, 24);
//	glDrawArrays(GL_LINES, 0, 12);
//	glBindVertexArray(0);

}

#endif
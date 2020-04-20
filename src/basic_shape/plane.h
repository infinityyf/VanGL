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

//terrain info
float* terrain;
int terrainSizeX;
int terrainSizeY;

class Plane {
public:
	unsigned int VBO, VAO;
	unsigned int textureID;
	unsigned int NormalTextureID;	//normal map(may not use)
	unsigned int HeightTextureID;	//height map(may not use)
	Plane(unsigned int textureID=NULL);
	void Draw(StandardShader* shader,int shadowID=NULL);
	void DrawDebug(StandardShader* shader);

	//generate terrain
	static void GenTerrain(int x=128, int y=128);

	glm::mat4 model;
};


Plane::Plane(unsigned int textureID) {
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if(terrain==nullptr)
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* terrainSizeX * terrainSizeY * 48, terrain, GL_STATIC_DRAW);

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
	if(terrain==nullptr)
		glDrawArrays(GL_PATCHES, 0, 24);
	else 
		glDrawArrays(GL_PATCHES, 0, terrainSizeX*terrainSizeY*6);
//	glDrawArrays(GL_LINES, 0, 12);
//	glBindVertexArray(0);

}

inline void Plane::GenTerrain(int x, int y)
{
	terrainSizeX = x;
	terrainSizeY = y;
	float deltaV = 5.0f / x;
	float deltaT = 1.0f / x;
	terrain = new float[x * y * 48];
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			//triangle 0
			terrain[i * y *48 + j * 48 + 0] = i*deltaV;
			terrain[i * y *48 + j * 48 + 1] = -0.5f;
			terrain[i * y *48 + j * 48 + 2] = j*deltaV;
						   
			terrain[i * y *48 + j * 48 + 3] = 0.0f;
			terrain[i * y *48 + j * 48 + 4] = 1.0f;
			terrain[i * y *48 + j * 48 + 5] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 6] = i*deltaT;
			terrain[i * y *48 + j * 48 + 7] = j*deltaT;
						   
			terrain[i * y *48 + j * 48 + 8] = (i+1) * deltaV;
			terrain[i * y *48 + j * 48 + 9] = -0.5f;
			terrain[i * y *48 + j * 48 + 10] = j * deltaV;
						   
			terrain[i * y *48 + j * 48 + 11] = 0.0f;
			terrain[i * y *48 + j * 48 + 12] = 1.0f;
			terrain[i * y *48 + j * 48 + 13] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 14] = (i+1) * deltaT;
			terrain[i * y *48 + j * 48 + 15] = j * deltaT;
						   
			terrain[i * y *48 + j * 48 + 16] = (i+1) * deltaV;
			terrain[i * y *48 + j * 48 + 17] = -0.5f;
			terrain[i * y *48 + j * 48 + 18] = (j+1) * deltaV;
						   
			terrain[i * y *48 + j * 48 + 19] = 0.0f;
			terrain[i * y *48 + j * 48 + 20] = 1.0f;
			terrain[i * y *48 + j * 48 + 21] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 22] = (i+1) * deltaT;
			terrain[i * y *48 + j * 48 + 23] = (j+1) * deltaT;
						   
			//triangle 1   
			terrain[i * y *48 + j * 48 + 24] = (i + 1) * deltaV;
			terrain[i * y *48 + j * 48 + 25] = -0.5f;
			terrain[i * y *48 + j * 48 + 26] = (j + 1) * deltaV;
						   
			terrain[i * y *48 + j * 48 + 27] = 0.0f;
			terrain[i * y *48 + j * 48 + 28] = 1.0f;
			terrain[i * y *48 + j * 48 + 29] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 30] = (i + 1) * deltaT;
			terrain[i * y *48 + j * 48 + 31] = (j + 1) * deltaT;
						   
			terrain[i * y *48 + j * 48 + 32] = i * deltaV;
			terrain[i * y *48 + j * 48 + 33] = -0.5f;
			terrain[i * y *48 + j * 48 + 34] = (j+1) * deltaV;
						   
			terrain[i * y *48 + j * 48 + 35] = 0.0f;
			terrain[i * y *48 + j * 48 + 36] = 1.0f;
			terrain[i * y *48 + j * 48 + 37] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 38] = i * deltaT;
			terrain[i * y *48 + j * 48 + 39] = (j+1) * deltaT;
						   
			terrain[i * y *48 + j * 48 + 40] = i * deltaV;
			terrain[i * y *48 + j * 48 + 41] = -0.5f;
			terrain[i * y *48 + j * 48 + 42] = j * deltaV;
						   
			terrain[i * y *48 + j * 48 + 43] = 0.0f;
			terrain[i * y *48 + j * 48 + 44] = 1.0f;
			terrain[i * y *48 + j * 48 + 45] = 0.0f;
						   
			terrain[i * y *48 + j * 48 + 46] = i * deltaT;
			terrain[i * y *48 + j * 48 + 47] = j * deltaT;
		}
	}

}

#endif
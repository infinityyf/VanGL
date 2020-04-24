#pragma once
#ifndef SKYBOX
#define SKYBOX
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"
#include <iostream>
#include <io.h>
#include <fstream>
#include <string>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../shader.h"
#include "../camera.h"

// skybox vertex
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

// read skybox textures and render it
class Skybox {
public:
	unsigned int skyBox;
	unsigned int skyVBO, skyVAO;
	StandardShader* shader;
	Skybox(std::string picDictionary);

public:
	void drawSkyBox();
	void drawSkyBox(unsigned int sky);
};

Skybox::Skybox(std::string picDictionary) {
	if (picDictionary == " ") {
		glGenTextures(1, &skyBox);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);

		intptr_t hFile = 0;
		std::vector<std::string> files;
		//file info from io.h
		struct _finddata_t fileinfo;
		std::string p;
		//use * to match all files the dictionary contains
		if ((hFile = _findfirst(p.assign(picDictionary).append("\\*").c_str(), &fileinfo)) != -1) {
			do {
				// skip a subdir (./ or ../)
				if (fileinfo.attrib & _A_SUBDIR) continue;
				files.push_back(p.assign(picDictionary).append("\\").append(fileinfo.name));
			} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
			_findclose(hFile);
		}
		int width, height, nrChannels;
		// load texture flip y coordinate
		stbi_set_flip_vertically_on_load(false);

		for (unsigned int i = 0; i < files.size(); i++)
		{
			std::cout << "load skybox images:" << files[i] << std::endl;
			unsigned char* data = stbi_load(files[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cerr << "ERROR: Cubemap texture failed to load at path: " << files[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glGenBuffers(1, &skyVBO);
	glGenVertexArrays(1, &skyVAO);

	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// load shader (this shader will be set as a default shader)
	shader = new StandardShader("E:\\vs_workspace\\VanGL\\src\\shaders\\skyboxShader\\skyBox.vs", "E:\\vs_workspace\\VanGL\\src\\shaders\\skyboxShader\\skyBox.fs");
	shader->use();
	shader->setInt("cubemap", 0);

}

inline void Skybox::drawSkyBox()
{
	shader->use();
	glBindVertexArray(skyVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

inline void Skybox::drawSkyBox(unsigned int sky)
{
	shader->use();
	glBindVertexArray(skyVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


#endif
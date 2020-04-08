#pragma once
#ifndef TEXTURE
#define TEXTURE
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"
#include <string>
#include <iostream>
#include <io.h>
#include <fstream>
#include <string>
#include <vector>

#include "shader.h"
#include "camera.h"

enum TEX_TYPE
{
	DIFFUSE_TEX = 0,
	SPECULAR_TEX = 1,
	AMBIENT_TEX = 2,
	NORMAL_TEX = 3,
	HEIGHT_TEX = 4,
	METALLIC_TEX = 5,
	ROUGHNESS_TEX = 6,
	AO_TEX = 7,
};

class Texture {
public:
	unsigned int textureID;

	// you can find mesh's texture by type so you can replace texture bt your own
	unsigned int textureType;

	Texture() {};
	Texture(const std::string picPath, GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter,GLint format);
	void setTextureType(unsigned int type);
	unsigned int loadTextureFromFile(const char* picName,std::string dictionary);
};

Texture::Texture(const std::string picPath, GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter,GLint format) {
	// generate texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// set texture wrap mode
	// set each coordinate s or t(function name means the value type set to texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

	// set texture filter mode
	// set min or mag filter function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);


	// load texture flip y coordinate
	stbi_set_flip_vertically_on_load(true);

	// load texture
	int ImageWidth, ImageHeight, nrChannels;
	unsigned char* data = stbi_load(picPath.c_str(), &ImageWidth, &ImageHeight, &nrChannels, 0);
	if (data) {
		// set date(set the second parameter to use mipmap or use glgenerateMipmap)
		glTexImage2D(GL_TEXTURE_2D, 0, format, ImageWidth, ImageHeight, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "ERROR: fail to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

inline void Texture::setTextureType(unsigned int type)
{
	textureType = type;
}

inline unsigned int Texture::loadTextureFromFile(const char* picName, std::string dictionary)
{
	
	std::string picPath = dictionary.append("\\").append(picName);
	std::cout << "load texture: "+ picPath << std::endl;

	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// load texture flip y coordinate
	stbi_set_flip_vertically_on_load(false);

	// load texture
	int ImageWidth, ImageHeight, nrChannels;
	unsigned char* data = stbi_load(picPath.c_str(), &ImageWidth, &ImageHeight, &nrChannels, 0);
	if (data) {
		//check the pic format(use channel)
		GLenum format = GL_RGB;
		if (nrChannels == 1) format = GL_RED;
		else if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;

		// set texture wrap mode
		// set each coordinate s or t(function name means the value type set to texture)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filter mode
		// set min or mag filter function
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// set date(set the second parameter to use mipmap or use glgenerateMipmap)
		glTexImage2D(GL_TEXTURE_2D, 0, format, ImageWidth, ImageHeight, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "ERROR: fail to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}
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
	if (picDictionary==" ") {
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
	shader->setInt("cubemap",0);
	
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
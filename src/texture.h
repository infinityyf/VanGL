#pragma once
#ifndef TEXTURE
#define TEXTURE
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <iostream>

#include <map>

enum TEX_TYPE
{
	DIFFUSE_TEX = 0,
	SPECULAR_TEX = 1,
	AMBIENT_TEX = 2,
	NORMAL_TEX = 3,
	HEIGHT_TEX = 4

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
	stbi_set_flip_vertically_on_load(true);

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

#endif
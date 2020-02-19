#pragma once
#ifndef TEXTURE
#define TEXTURE
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <iostream>

class Texture {
public:
	unsigned int textureID;
	Texture(const std::string picPath, GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter,GLint format);
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

#endif
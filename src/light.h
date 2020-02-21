#pragma once
#ifndef LIGHT
#define LIGHT
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "shader.h"
class LightBase {
public:
	glm::vec4 lightPos;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	//basic light info for shadering
	Shader* lightShader;
	unsigned int lightID;
	LightBase(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};
LightBase::LightBase(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}


class DirectionLight : LightBase {
public:
	glm::vec3 direction;
	
};

class PointLight : LightBase {
public:
	float constant;
	float linear;
	float quadratic;
};

class SpotLight : LightBase {
public:
	glm::vec3 direction;
	float innerCutoff;
	float outerCutOff;
};

class LightManager {
public:
	std::vector<LightBase> lights;
};

#endif
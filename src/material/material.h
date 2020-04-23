#pragma once
#ifndef MATERIAL
#define MATERIAL
#include <glad/glad.h>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


class ForwardStandardMaterial {
public:
	//texture info
	unsigned int diffuseTexture;
	unsigned int specularTexture;
	unsigned int normalTexture;
	unsigned int ambientTexture;
	unsigned int skyBox;

	//program info
	unsigned int vertexProgram;
	unsigned int fragmentProgram;
	unsigned int tcProgram;
	unsigned int teProgram;
	unsigned int geomertryProgram;

	//pipeline info
	unsigned int renderingPipeline;

public:
	ForwardStandardMaterial();
	void setBool(const std::string& name, bool value, unsigned int programID)
	{
		glUseProgram(programID);
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value, unsigned int programID)
	{
		glUseProgram(programID);
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value, unsigned int programID)
	{
		glUseProgram(programID);
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}

	void setMatrix4(const std::string& name, const glm::mat4 matrix, unsigned int programID)
	{
		glUseProgram(programID);
		unsigned int matrixLocation = glGetUniformLocation(programID, name.c_str());
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void setVector3(const std::string& name, const glm::vec3 vec, unsigned int programID)
	{
		glUseProgram(programID);
		unsigned int location = glGetUniformLocation(programID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(vec));
	}

};


class ForwardPBRMaterial {
public:
	unsigned int albedoTexture;
	unsigned int roughnessTexture;
	unsigned int normalTexture;
	unsigned int metallicTexture;
	unsigned int aoTexture;
};

class DeferredStandardMaterial {
public:
	unsigned int diffuseTexture;
	unsigned int specularTexture;
	unsigned int normalTexture;
	unsigned int ambientTexture;
	unsigned int skyBox;
};

#endif // !MATERIAL

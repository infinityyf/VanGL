#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm.hpp>
// matrix operation
#include <gtc/matrix_transform.hpp>
// send data pointer
#include <gtc/type_ptr.hpp>
/*
stream's function can return  different status bit
eofbit: end of source
failbit: the input obtained could not be interpreted as a valid representaion
badbit: error occured
*/
#include <string>
#include <fstream>	// file IO
#include <sstream>	// string IO
#include <iostream> // IO


class StandardShader
{
public:
	// shader ID
	unsigned int shaderProgramID;

	// get source code 
	StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// use program
	void use();

	// set uniform (const means it doesnt change any data in this class)
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4(const std::string& name,const glm::mat4 matrix) const;
	void setVector3(const std::string& name, const glm::vec3 vec)const;
};

StandardShader::StandardShader(const char* vertexPath, const char* fragmentPath) {
	//open file
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//set exception bit(use bit operator)
	//this function can get or set exception mask
	//then we can use try catch
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//stream has 4 bit: goodbit, eofbit, failbit, badbit
	//when goodbit is false stream will close
	try {
		//open file
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		//rdbuf: use another stream to output this stream content
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure error){
		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
	}

	//source code
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	//compile vertex shader runtime
	unsigned int vertexShader;
	//compile fragment shader runtime
	unsigned int fragmentShader;
	//check result
	int  success;
	char infoLog[512];



	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << std::endl;
	}

	
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
	}

	//link shader to shader program object. it will be activated when rendering
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void StandardShader::use() {
	glUseProgram(shaderProgramID);
}

inline void StandardShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glad_glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
}

inline void StandardShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

inline void StandardShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

inline void StandardShader::setMatrix4(const std::string& name, const glm::mat4 matrix) const
{
	unsigned int matrixLocation = glGetUniformLocation(shaderProgramID, name.c_str());
	// second parameter means how many matrixs
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

inline void StandardShader::setVector3(const std::string& name, const glm::vec3 vec) const
{
	unsigned int location = glGetUniformLocation(shaderProgramID, name.c_str());
	glUniform3fv(location, 1, glm::value_ptr(vec));
}


#endif
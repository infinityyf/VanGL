#pragma once
#ifndef MATERIAL
#define MATERIAL
#include <glad/glad.h>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../texture.h"

enum SHADER_TYPE {
	NULL_SHADER = -1,
	VERTEX_SHADER = GL_VERTEX_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
	TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
	TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
	GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	COMPUTE_SHADER = GL_COMPUTE_SHADER,
};



class BaseMaterial {
public:
	static unsigned int MaterixUBO;
public:
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
	//get single shader program
	static unsigned int CreateShader(const GLchar* filePath, unsigned int shaderType) {

		//for include
		const GLchar* shaderSearchPath = "/";
		//for tessellation 
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		std::string shaderCode;
		std::ifstream shaderFile;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			//open file
			shaderFile.open(filePath);
			std::stringstream shaderStream;

			//rdbuf: use another stream to output this stream content
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();

			shaderCode = shaderStream.str();

		}
		catch (std::ifstream::failure error) {
			std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
		}

		//source code
		const char* shaderCodeInC = shaderCode.c_str();

		//compile vertex shader runtime
		unsigned int shader;

		//check result
		int  success;
		char infoLog[512];

		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCodeInC, NULL);
		glCompileShaderIncludeARB(shader, 1, &shaderSearchPath, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << std::endl;
			return -1;
		}

		//attach shader to a program
		GLuint program = glCreateProgram();
		glAttachShader(program, shader);
		glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);	//set as seperate
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
		}
		glDeleteShader(shader);
		return program;
	}
	//create include file
	static void CreateIncludeShaderFile(const GLchar* includePath) {
		std::string filePath = includePath;
		std::string shaderCode;
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			//open file
			shaderFile.open(filePath);
			std::stringstream shaderStream;

			//rdbuf: use another stream to output this stream content
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();

			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure error) {
			std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
		}
		//get include shader filename
		std::string fileName = filePath.substr(filePath.find_last_of('\\') + 1, filePath.length() - filePath.find_last_of('\\'));
		std::string shaderFileName = "/" + fileName;
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, shaderFileName.size(), shaderFileName.c_str(), shaderCode.size(), shaderCode.c_str());
	}
	//create ubo  update uniform data
	static void createUBOBinding() {
		glGenBuffers(1, &MaterixUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, MaterixUBO);
		//use this function to allocate memory
		glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//bind uniform buffer object to bind point
		glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, MaterixUBO);
	}
};

class ForwardStandardMaterial : public BaseMaterial {

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
	unsigned int geometryProgram;

	//pipeline info
	unsigned int renderingPipeline;

public:
	ForwardStandardMaterial(unsigned int vertex, unsigned int fragment, unsigned int tessellationControl, unsigned int tessellationEvaluation, unsigned int geometry) {
		vertexProgram = vertex;
		fragmentProgram = fragment;
		tcProgram = tessellationControl;
		teProgram = tessellationEvaluation;
		geometryProgram = geometry;

		diffuseTexture = NULL_TEX;
		specularTexture = NULL_TEX;
		normalTexture = NULL_TEX;
		ambientTexture = NULL_TEX;
		skyBox = NULL_TEX;

		glGenProgramPipelines(1, &renderingPipeline);
		if (vertexProgram != NULL_SHADER) {
			//binding to UBO
			glUseProgramStages(renderingPipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
			unsigned int matrixIndex = glGetUniformBlockIndex(vertexProgram, "Matrix");
			glUniformBlockBinding(vertexProgram, matrixIndex, BIND_POINT::MATRIX_POINT);
		}
			
		if (fragmentProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
		if (tcProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_TESS_CONTROL_SHADER_BIT, tcProgram);
		if (teProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_TESS_EVALUATION_SHADER_BIT, teProgram);
		if (geometryProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_GEOMETRY_SHADER_BIT, geometryProgram);
	}

	void setTexture(unsigned int textureID, int textureType) {
		switch (textureType)
		{
		case DIFFUSE_TEX:	diffuseTexture = textureID; break;
		case SPECULAR_TEX:	specularTexture = textureID; break;
		case AMBIENT_TEX:	ambientTexture = textureID; break;
		case NORMAL_TEX:	normalTexture = textureID; break;
		case SKYBOX_TEX:	skyBox = textureID; break;
		default:
			break;
		}
	}

	void setProgram(unsigned int program,int programType) {
		switch (programType)
		{
		case VERTEX_SHADER:				vertexProgram = program; break;
		case FRAGMENT_SHADER:			fragmentProgram = program; break;
		case TESS_CONTROL_SHADER:		tcProgram = program; break;
		case TESS_EVALUATION_SHADER:	teProgram = program; break;
		case GEOMETRY_SHADER:			geometryProgram = program; break;
		default:
			break;
		}
	}

	void use() {
		glBindProgramPipeline(renderingPipeline);
		if (vertexProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
		if (fragmentProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
		if (tcProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_TESS_CONTROL_SHADER_BIT, tcProgram);
		if (teProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_TESS_EVALUATION_SHADER_BIT, teProgram);
		if (geometryProgram != NULL_SHADER)
			glUseProgramStages(renderingPipeline, GL_GEOMETRY_SHADER_BIT, geometryProgram);
	}

	
};


class ForwardPBRMaterial : public BaseMaterial {
public:
	unsigned int albedoTexture;
	unsigned int roughnessTexture;
	unsigned int normalTexture;
	unsigned int metallicTexture;
	unsigned int aoTexture;
};

class DeferredStandardMaterial : public BaseMaterial {
public:
	unsigned int diffuseTexture;
	unsigned int specularTexture;
	unsigned int normalTexture;
	unsigned int ambientTexture;
	unsigned int skyBox;
};

class CustomMaterial : public BaseMaterial {

};

class SkyBoxMaterial : public BaseMaterial {
public:
	unsigned int skyBox;


};

#endif // !MATERIAL
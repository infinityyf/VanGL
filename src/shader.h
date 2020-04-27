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


enum BIND_POINT {
	MATRIX_POINT = 0,
	SSAO_SAMPLER_POINT = 1,
};

enum SHADER_TYPE {
	NULL_SHADER = -1,
	VERTEX_SHADER = GL_VERTEX_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
	TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
	TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
	GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	COMPUTE_SHADER = GL_COMPUTE_SHADER,
};

//for rendering functional things
class StandardShader
{
public:
	// called once before use shader
	static void createIncludeShaderFile(const GLchar* includePath);
	const GLchar* shaderSearchPath;
	
	// shader ID
	unsigned int shaderProgramID;

	// get source code 
	StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr,bool include = false);
	StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tessellationTCL, const GLchar* tessellationTEL, bool include = false);
	StandardShader(const GLchar* computePath);
	
	// use program
	void use();

	// set uniform (const means it doesnt change any data in this class)
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4(const std::string& name,const glm::mat4 matrix) const;
	void setVector3(const std::string& name, const glm::vec3 vec)const;

	//use a texture(define a new texture unit)
	void useTexture(const std::string& shaderName);
};

inline StandardShader::StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath, bool include)
{

	shaderSearchPath = "/";
	//open file
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	std::string geometryCode;
	std::ifstream gShaderFile;

	//set exception bit(use bit operator)
	//this function can get or set exception mask
	//then we can use try catch
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

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

		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}

	}
	catch (std::ifstream::failure error) {
		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
	}

	//source code
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	//compile vertex shader runtime
	unsigned int vertexShader;
	//compile fragment shader runtime
	unsigned int fragmentShader;
	unsigned int geometryShader;

	//check result
	int  success;
	char infoLog[512];

	
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	//add include shader
	if (include) {
		glCompileShaderIncludeARB(vertexShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << std::endl;
	}


	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	if (include) {
		glCompileShaderIncludeARB(fragmentShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
	}	
	
	
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &gShaderCode, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
		}
	}

	//link shader to shader program object. it will be activated when rendering
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	if (geometryPath != nullptr) glAttachShader(shaderProgramID, geometryShader);
	glLinkProgram(shaderProgramID);
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryPath != nullptr) glDeleteShader(geometryShader);

	
}

inline StandardShader::StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tessellationTCL, const GLchar* tessellationTEL, bool include)
{
	shaderSearchPath = "/";
	//set tesselation patch vertices number
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	//open file
	std::string vertexCode;
	std::string fragmentCode;
	std::string tcsCode;
	std::string tesCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream tcsShaderFile;
	std::ifstream tesShaderFile;


	//set exception bit(use bit operator)
	//this function can get or set exception mask
	//then we can use try catch
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tcsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tesShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	//stream has 4 bit: goodbit, eofbit, failbit, badbit
	//when goodbit is false stream will close
	try {
		//open file
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		tcsShaderFile.open(tessellationTCL);
		tesShaderFile.open(tessellationTEL);


		std::stringstream vShaderStream, fShaderStream,tcsShaderStream,tesShaderStream;

		//rdbuf: use another stream to output this stream content
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		tcsShaderStream << tcsShaderFile.rdbuf();
		tesShaderStream << tesShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		tcsShaderFile.close();
		tesShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		tcsCode = tcsShaderStream.str();
		tesCode = tesShaderStream.str();

	}
	catch (std::ifstream::failure error) {
		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
	}

	//source code
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* tcsShaderCode = tcsCode.c_str();
	const char* tesShaderCode = tesCode.c_str();


	//compile vertex shader runtime
	unsigned int vertexShader;
	//compile fragment shader runtime
	unsigned int fragmentShader;
	unsigned int tcsShader;
	unsigned int tesShader;

	//check result
	int  success;
	char infoLog[512];


	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	//add include shader
	if (include) {
		glCompileShaderIncludeARB(vertexShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << std::endl;
	}


	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	if (include) {
		glCompileShaderIncludeARB(fragmentShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
	}

	tcsShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tcsShader, 1, &tcsShaderCode, NULL);
	if (include) {
		glCompileShaderIncludeARB(tcsShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(tcsShader);
	glGetShaderiv(tcsShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(tcsShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN TCS SHADER:\n" << infoLog << std::endl;
	}


	tesShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tesShader, 1, &tesShaderCode, NULL);
	if (include) {
		glCompileShaderIncludeARB(tesShader, 1, &shaderSearchPath, nullptr);
	}
	glCompileShader(tesShader);
	glGetShaderiv(tesShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(tesShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN TES SHADER:\n" << infoLog << std::endl;
	}


	//link shader to shader program object. it will be activated when rendering
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glAttachShader(shaderProgramID, tcsShader);
	glAttachShader(shaderProgramID, tesShader);
	
	glLinkProgram(shaderProgramID);
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



}

inline StandardShader::StandardShader(const GLchar* computePath)
{
	//open file
	std::string computeCode;
	std::ifstream computeShaderFile;
	computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		//open file
		computeShaderFile.open(computePath);
		std::stringstream computeShaderStream, fShaderStream;

		//rdbuf: use another stream to output this stream content
		computeShaderStream << computeShaderFile.rdbuf();
		computeShaderFile.close();

		computeCode = computeShaderStream.str();

	}
	catch (std::ifstream::failure error) {
		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
	}

	//source code
	const char* computeShaderCode = computeCode.c_str();


	unsigned int computeShader;

	//check result
	int  success;
	char infoLog[512];


	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &computeShaderCode, NULL);

	glCompileShader(computeShader);

	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
		std::cerr << "ERROR IN compute SHADER:\n" << infoLog << std::endl;
	}

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, computeShader);
	glLinkProgram(shaderProgramID);
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
	}
	glDeleteShader(computeShader);
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

void StandardShader::createIncludeShaderFile(const GLchar* includePath)
{

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
	std::string fileName = filePath.substr(filePath.find_last_of('\\')+1, filePath.length() - filePath.find_last_of('\\'));
	std::string shaderFileName = "/" + fileName;
	glNamedStringARB(GL_SHADER_INCLUDE_ARB, shaderFileName.size(), shaderFileName.c_str(), shaderCode.size(), shaderCode.c_str());
}

//get single shader program
unsigned int CreateShader(const GLchar* filePath, unsigned int shaderType) {

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



//seperate shader pipeline for rendering items
class SeperatePipeline {
public:
	// shader ID
	unsigned int shaderPipeLineID;
	//include path
	const GLchar* shaderSearchPath;

	//components
	unsigned int vertexProgram;
	unsigned int fragmentProgram;
	unsigned int tessellationControlProgram;
	unsigned int tessellationEvaluationProgram;
	unsigned int geometryProgram;

	SeperatePipeline(
		unsigned int vertex, 
		unsigned int fragment, 
		unsigned int tessellationControl, 
		unsigned int tessellationEvaluation, 
		unsigned int geometry);

	// use program
	void use();
	// called once before use shader
	static void createIncludeShaderFile(const GLchar* includePath);

	
};

//create pipeline record every program
inline SeperatePipeline::SeperatePipeline(unsigned int vertex, unsigned int fragment, unsigned int tessellationControl, unsigned int tessellationEvaluation, unsigned int geometry)
{
	glGenProgramPipelines(1, &shaderPipeLineID);
	glBindProgramPipeline(shaderPipeLineID);
	if (vertex != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_VERTEX_SHADER_BIT, vertex);
	if (fragment != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_FRAGMENT_SHADER_BIT, fragment);
	if (tessellationControl != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_TESS_CONTROL_SHADER_BIT, tessellationControl);
	if (tessellationEvaluation != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_TESS_EVALUATION_SHADER_BIT, tessellationEvaluation);
	if (geometry != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_GEOMETRY_SHADER_BIT, geometry);
	this->vertexProgram = vertex;
	this->fragmentProgram = fragment;
	this->tessellationControlProgram = tessellationControl;
	this->tessellationEvaluationProgram = tessellationEvaluation;
	this->geometryProgram = geometry;
}

//assemble the pipeline
inline void SeperatePipeline::use() {
	glBindProgramPipeline(shaderPipeLineID);
	if (vertexProgram != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_VERTEX_SHADER_BIT, vertexProgram);
	if (fragmentProgram != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
	if (tessellationControlProgram != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_TESS_CONTROL_SHADER_BIT, tessellationControlProgram);
	if (tessellationEvaluationProgram != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_TESS_EVALUATION_SHADER_BIT, tessellationEvaluationProgram);
	if (geometryProgram != NULL_SHADER)
		glUseProgramStages(shaderPipeLineID, GL_GEOMETRY_SHADER_BIT, geometryProgram);

}

inline void SeperatePipeline::createIncludeShaderFile(const GLchar* includePath)
{
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

inline void setBool(const std::string& name, bool value, unsigned int programID)
{
	glUseProgram(programID);
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

inline void setInt(const std::string& name, int value, unsigned int programID)
{
	glUseProgram(programID);
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

inline void setFloat(const std::string& name, float value, unsigned int programID)
{
	glUseProgram(programID);
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

inline void setMatrix4(const std::string& name, const glm::mat4 matrix, unsigned int programID)
{
	glUseProgram(programID);
	unsigned int matrixLocation = glGetUniformLocation(programID, name.c_str());
	// second parameter means how many matrixs
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

inline void setVector3(const std::string& name, const glm::vec3 vec, unsigned int programID)
{
	glUseProgram(programID);
	unsigned int location = glGetUniformLocation(programID, name.c_str());
	glUniform3fv(location, 1, glm::value_ptr(vec));
}



#endif
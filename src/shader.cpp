//#include "shader.h"
//
//inline StandardShader::StandardShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath, bool include)
//{
//	//open file
//	std::string vertexCode;
//	std::string fragmentCode;
//	std::ifstream vShaderFile;
//	std::ifstream fShaderFile;
//
//	std::string geometryCode;
//	std::ifstream gShaderFile;
//
//	//set exception bit(use bit operator)
//	//this function can get or set exception mask
//	//then we can use try catch
//	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//
//	//stream has 4 bit: goodbit, eofbit, failbit, badbit
//	//when goodbit is false stream will close
//	try {
//		//open file
//		vShaderFile.open(vertexPath);
//		fShaderFile.open(fragmentPath);
//		std::stringstream vShaderStream, fShaderStream;
//
//		//rdbuf: use another stream to output this stream content
//		vShaderStream << vShaderFile.rdbuf();
//		fShaderStream << fShaderFile.rdbuf();
//		vShaderFile.close();
//		fShaderFile.close();
//
//		vertexCode = vShaderStream.str();
//		fragmentCode = fShaderStream.str();
//
//		if (geometryPath != nullptr)
//		{
//			gShaderFile.open(geometryPath);
//			std::stringstream gShaderStream;
//			gShaderStream << gShaderFile.rdbuf();
//			gShaderFile.close();
//			geometryCode = gShaderStream.str();
//		}
//
//	}
//	catch (std::ifstream::failure error) {
//		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
//	}
//
//	//source code
//	const char* vShaderCode = vertexCode.c_str();
//	const char* fShaderCode = fragmentCode.c_str();
//
//
//	//compile vertex shader runtime
//	unsigned int vertexShader;
//	//compile fragment shader runtime
//	unsigned int fragmentShader;
//	unsigned int geometryShader;
//
//	//check result
//	int  success;
//	char infoLog[512];
//
//
//	vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
//	//add include shader
//	if (include) {
//		glCompileShaderIncludeARB(vertexShader, 1, &shaderSearchPath, nullptr);
//	}
//	glCompileShader(vertexShader);
//
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//		std::cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << std::endl;
//	}
//
//
//	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
//	if (include) {
//		glCompileShaderIncludeARB(fragmentShader, 1, &shaderSearchPath, nullptr);
//	}
//	glCompileShader(fragmentShader);
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//		std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
//	}
//
//
//	if (geometryPath != nullptr)
//	{
//		const char* gShaderCode = geometryCode.c_str();
//		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
//		glShaderSource(geometryShader, 1, &gShaderCode, NULL);
//		glCompileShader(geometryShader);
//		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
//		if (!success) {
//			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
//			std::cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << std::endl;
//		}
//	}
//
//	//link shader to shader program object. it will be activated when rendering
//	shaderProgramID = glCreateProgram();
//	glAttachShader(shaderProgramID, vertexShader);
//	glAttachShader(shaderProgramID, fragmentShader);
//	if (geometryPath != nullptr) glAttachShader(shaderProgramID, geometryShader);
//	glLinkProgram(shaderProgramID);
//	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
//	if (!success) {
//		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
//		std::cerr << "ERROR IN LINK:\n" << infoLog << std::endl;
//	}
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//	if (geometryPath != nullptr) glDeleteShader(geometryShader);
//}
//
//void StandardShader::use() {
//	glUseProgram(shaderProgramID);
//}
//
//inline void StandardShader::setBool(const std::string& name, bool value) const
//{
//	glUniform1i(glad_glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
//}
//
//inline void StandardShader::setInt(const std::string& name, int value) const
//{
//	glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
//}
//
//inline void StandardShader::setFloat(const std::string& name, float value) const
//{
//	glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
//}
//
//inline void StandardShader::setMatrix4(const std::string& name, const glm::mat4 matrix) const
//{
//	unsigned int matrixLocation = glGetUniformLocation(shaderProgramID, name.c_str());
//	// second parameter means how many matrixs
//	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
//}
//
//inline void StandardShader::setVector3(const std::string& name, const glm::vec3 vec) const
//{
//	unsigned int location = glGetUniformLocation(shaderProgramID, name.c_str());
//	glUniform3fv(location, 1, glm::value_ptr(vec));
//}
//
//void StandardShader::createIncludeShaderFile()
//{
//	includePath = "src\\shaders\\includeShader\\StandardShader.glsl";
//	shaderSearchPath = "/";
//
//	std::string filePath = includePath;
//
//	std::string shaderCode;
//	std::ifstream shaderFile;
//	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//	try {
//		//open file
//		shaderFile.open(filePath);
//		std::stringstream shaderStream;
//
//		//rdbuf: use another stream to output this stream content
//		shaderStream << shaderFile.rdbuf();
//		shaderFile.close();
//
//		shaderCode = shaderStream.str();
//	}
//	catch (std::ifstream::failure error) {
//		std::cerr << "ERROR SHADER: FILE READ FAILE" << std::endl;
//	}
//	//get include shader filename
//	std::string fileName = filePath.substr(filePath.find_last_of("/"), filePath.length() - filePath.find_last_of("/"));
//	std::string shaderFileName = "/" + fileName + ".glsl";
//	glNamedStringARB(GL_SHADER_INCLUDE_ARB, shaderFileName.size(), shaderFileName.c_str(), shaderCode.size(), shaderCode.c_str());
//}

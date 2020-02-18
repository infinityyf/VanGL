#include "contextManager.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	//get key press event
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

float vertices[] = {
	  0.5f,  0.5f, 0.0f,  // top right
	  0.5f, -0.5f, 0.0f,  // bottom right
	 -0.5f, -0.5f, 0.0f,  // bottom left
	 -0.5f,  0.5f, 0.0f   // top left 
};

unsigned int indices[] = { // 注意索引从0开始! 
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};



int main() {

	

	GLFWwindow* window;
	int width = 800;
	int height = 600;

	if (InitGLFWandGLAD(&window, width, height) != 0)
		return -1;

	//register size change call back
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//use VAO to record vertex attributes it can only contain 16 attributes
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//generate buffer and bind to gl_array_buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	
	//then vertex attribute will bind to VAO 
	//just change different VAO when rendering different objects
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//GL_STATIC_DRAW : data wont change
	//GL_DYNAMIC_DRAW : data change a lot
	//GL_STREAM_DRAW : data change every time
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//also record in VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//how to resolve tha vertex data
	//first 0 means location 0 in vertex shader
	//GL_FALSE means we dont want map data between 0,1 or -1,1
	//this funciton will run above VBO bind to GL_ARRAY_BUFFER
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	//donnt unbind VEO before VAO is active
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//shader source
	const char* vertexShaderSource = "#version 330 core \n \
		layout(location = 0) in vec3 aPos;void main(){gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}";
	const char* fragmentShaderSource = "#version 330 core \n \
		out vec4 FragColor;void main(){FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}";

	//compile vertex shader runtime
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//check result
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cerr << "ERROR IN VERTEX SHADER:\n" << infoLog << endl;
	}

	//compile fragment shader runtime
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cerr << "ERROR IN FRAGMENT SHADER:\n" << infoLog << endl;
	}

	//link shader to shader program object. it will be activated when rendering
	unsigned int shaderProgramObject;
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShader);
	glAttachShader(shaderProgramObject, fragmentShader);
	glLinkProgram(shaderProgramObject);
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramObject, 512, NULL, infoLog);
		cerr << "ERROR IN LINK:\n" << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	

	//render loop
	while (!glfwWindowShouldClose(window)) {
		

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//use shaders link to this program object
		glUseProgram(shaderProgramObject);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		//check events
		glfwPollEvents();
		processInput(window);
		glfwSwapBuffers(window);
	}
	
	//release resources
	glfwTerminate();

	return 0;
}
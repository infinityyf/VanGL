#include "stdafx.h"
#include "shader.h"
#include "window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void processInput(GLFWwindow* window) {
	//get key press event
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

float vertices[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

unsigned int indices[] = { // 注意索引从0开始! 
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};




int main() {

	int width = 800;
	int height = 600;

	Window window(width, height);
	if (window.window == NULL) return -1;

	//register size change call back
	window.SetFramebufferSizeCallback(framebuffer_size_callback);

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
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//donnt unbind VEO before VAO is active
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	// generate texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set texture wrap mode
	// set each coordinate s or t(function name means the value type set to texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// set texture filter mode
	// set min or mag filter function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load texture
	int ImageWidth, ImageHeight, nrChannels;
	unsigned char* data = stbi_load("scene\\materials\\textures\\container.jpg", &ImageWidth, &ImageHeight, &nrChannels, 0);
	if (data) {
		// set date(set the second parameter to use mipmap or use glgenerateMipmap)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "ERROR: fail to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D,0);


	Shader shader("src\\shaders\\shader.vs", "src\\shaders\\shader.fs");

	//render loop
	while (!glfwWindowShouldClose(window.window)) {
		

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//use shaders link to this program object
		shader.use();

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		//check events
		glfwPollEvents();
		processInput(window.window);
		glfwSwapBuffers(window.window);
	}
	
	//release resources
	glfwTerminate();

	return 0;
}
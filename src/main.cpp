#include "stdafx.h"
#include "shader.h"
#include "window.h"
#include "texture.h"
#include "camera.h"




float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
	//get cursor but make it invisible
	window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera.linkToWindow(&window);
	//register size change call back
	camera.SetFramebufferSizeCallback();
	camera.SetCursorCallBack();
	camera.SetScrollCallBack();

	

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//donnt unbind VEO before VAO is active
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//create light VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// data has already be binded
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// path wrap_s wrap_t min_filter mag_filter format
	Texture texture1("scene\\materials\\textures\\container.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB);
	Texture texture2("scene\\materials\\textures\\awesomeface.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	Shader shader("src\\shaders\\shader.vs", "src\\shaders\\shader.fs");
	Shader lightShader("src\\shaders\\lightShader.vs", "src\\shaders\\lightShader.fs");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, glm::vec3(1.0f));
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	
	//enable z buffer test
	glEnable(GL_DEPTH_TEST);


	//render loop
	while (!glfwWindowShouldClose(window.window)) {
		float currentFrame = glfwGetTime();
		camera.deltaTime = currentFrame - camera.lastFrame;
		camera.lastFrame = currentFrame;
		camera.processInput();
		camera.updateMatrixs();


		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		
		
		//use shaders link to this program object
		shader.use();
		// set texture1 uniform to texture unit0
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);
		shader.setMatrix4("model", model);
		shader.setMatrix4("view", camera.view);
		shader.setMatrix4("projection", camera.projection);
		shader.setVector3("lightColor", glm::vec3(0.6f,0.5f,0.0f));
		glActiveTexture(GL_TEXTURE0);
		// bind texture to texture unit0
		glBindTexture(GL_TEXTURE_2D, texture1.textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2.textureID);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//use shaders link to this program object
		lightShader.use();
		lightShader.setMatrix4("model", lightModel);
		lightShader.setMatrix4("view", camera.view);
		lightShader.setMatrix4("projection", camera.projection);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//check events
		glfwPollEvents();
		glfwSwapBuffers(window.window);


	}
	
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	//release resources
	glfwTerminate();

	return 0;
}
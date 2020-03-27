//#include "stdafx.h"
//// content setting
//#include "window.h"
//#include "texture.h"
//
////rendering
//#include "camera.h"
//#include "shader.h"
//
////basic shape
//#include "basic_shape/plane.h"
//#include "basic_shape/line.h"
//
////particles
//#include "particle_system/particles.h"
//
//std::string path = "E:\\vs_workspace\\VanGL\\";
//
////render setting
//bool BLOOM_ENABLE = false;
//bool DEFERRED_RENDERING = true;
//
//
//int main() {
//	int width = 800;
//	int height = 600;
//	Window window(width, height);
//	if (window.window == NULL) return -1;
//	//get cursor but make it invisible
//	window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	Camera camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 100.0f);
//	camera.linkToWindow(&window);
//	//register size change call back
//	camera.SetFramebufferSizeCallback();
//	camera.SetCursorCallBack();
//	camera.SetScrollCallBack();
//
//	Particles particles(10000);
//	glm::vec3 start(0.0f, 2.0f, 2.0f);
//	glm::vec3 end(0.0f, -3.0f, -5.0f);
//	Line line = Line(start, end);
//	StandardShader particleShader((path + "src\\shaders\\particleShader\\particle.vs").c_str(), (path + "src\\shaders\\particleShader\\particle.fs").c_str());
//
//	unsigned int UBO;
//	glGenBuffers(1, &UBO);
//	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
//	//use this function to allocate memory
//	glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_UNIFORM_BUFFER,0);
//
//	unsigned int samplerIndex1 = glGetUniformBlockIndex(particleShader.shaderProgramID, "Matrix");
//	glUniformBlockBinding(particleShader.shaderProgramID, samplerIndex1, BIND_POINT::MATRIX_POINT);
//	glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, UBO);
//
//	//enable z buffer test
//	glEnable(GL_DEPTH_TEST);
//	//glEnable(GL_STENCIL_TEST);
//	glEnable(GL_CULL_FACE);
//	//glCullFace(GL_FRONT);
//	//enable multisample
//	glEnable(GL_MULTISAMPLE);
//
//
//	//render loop
//	while (!glfwWindowShouldClose(window.window)) {
//		float currentFrame = glfwGetTime();
//		camera.deltaTime = currentFrame - camera.lastFrame;
//		camera.lastFrame = currentFrame;
//		camera.processInput();
//		camera.updateMatrixs();
//
//		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
//		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(camera.view));
//		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(camera.projection));
//		glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &camera.nearPlane);
//		glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &camera.farPlane);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//		
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glEnable(GL_DEPTH_TEST);
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		//send to solve and draw
//		particles.uploadToODE();
//		particles.drawPaticles(&particleShader);
//
//
//		glfwPollEvents();
//		glfwSwapBuffers(window.window);
//	}
//
//	//release resources
//	glfwTerminate();
//
//	return 0;
//}

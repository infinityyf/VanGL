#include "stdafx.h"
// content setting
#include "window.h"
#include "texture.h"

//rendering
#include "camera.h"
#include "shader.h"
#include "shadowMap.h"
#include "model_loader/model.h"

//basic shape
#include "basic_shape/plane.h"
#include "basic_shape/line.h"

//cube map
#include "light_probe/cubeMapGenerate.h"

//gui
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"


//render setting
bool BLOOM_ENABLE = false;
bool DEFERRED_RENDERING = false;
std::string path = "E:\\vs_workspace\\VanGL\\";




int main() {
	int width = 800;
	int height = 600;
	Window window(width, height);
	if (window.window == NULL) return -1;
	//get cursor but make it invisible
	//window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.01f, 100.0f);
	camera.linkToWindow(&window);
	//register size change call back
	camera.SetFramebufferSizeCallback();
	camera.SetCursorCallBack();
	camera.SetScrollCallBack();

	


	//create computer shader
	//init include shaders
	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseLights.glsl").c_str());
	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseFunctions.glsl").c_str());
	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseVariables.glsl").c_str());
	Skybox skybox(path + "scene\\materials\\textures\\skybox");
	StandardShader rayTracingShader((path + "src\\shaders\\computeShader\\rayTracing.comp").c_str());
	rayTracingShader.use();
	rayTracingShader.setInt("sky", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.skyBox);
	StandardShader postShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\postProcessShader.fs").c_str());
	postShader.use();
	postShader.setInt("screenTexture", 0);
	
	
	//generate mesh info send to computer shader



	//load model
	Model dragon((path + "scene\\models\\dragon.obj"));
	


	//createimage
	unsigned int screen;
	glGenTextures(1, &screen);
	glBindTexture(GL_TEXTURE_2D, screen);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, width, height);

	glBindImageTexture(0, screen, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindTexture(GL_TEXTURE_2D, 0);

	//post rendering
	Screen* postScreen = new Screen();
	
	float* data = new float[width * height];

	//render loop
	while (!glfwWindowShouldClose(window.window)) {
		float currentFrame = glfwGetTime();
		camera.deltaTime = currentFrame - camera.lastFrame;
		camera.lastFrame = currentFrame;
		camera.processInput();
		camera.updateMatrixs();

		//calculate rays
		rayTracingShader.use();
		rayTracingShader.setVector3("viewPos", camera.cameraPos);
		glBindImageTexture(0, screen, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

		glDispatchCompute(width, height,1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);



		postScreen->Draw(&postShader, screen);

		glfwPollEvents();
		glfwSwapBuffers(window.window);
	}


	glfwTerminate();

	return 0;
}

#include "stdafx.h"
// content setting
#include "window.h"
#include "texture.h"

//rendering
#include "camera.h"
#include "model.h"
#include "shader.h"


std::string path = "E:\\vs_workspace\\VanGL\\";

int main() {

	//HANDLE handle;
	//handle = GetStdHandle(STD_ERROR_HANDLE);//get handler of standars error output
	//SetConsoleTextAttribute(handle, 0xF);//set color

	int width = 800;
	int height = 600;
	Window window(width, height);
	if (window.window == NULL) return -1;
	//get cursor but make it invisible
	window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),0.1f,100.0f);
	camera.linkToWindow(&window);
	//register size change call back
	camera.SetFramebufferSizeCallback();
	camera.SetCursorCallBack();
	camera.SetScrollCallBack();


	// path wrap_s wrap_t min_filter mag_filter format
	Texture texture1(path+"scene\\materials\\textures\\container.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB);
	Texture texture2(path+"scene\\materials\\textures\\awesomeface.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	Texture diffuseTexture(path+"scene\\materials\\textures\\container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	diffuseTexture.setTextureType(DIFFUSE_TEX);
	Texture specularTexture(path+"scene\\materials\\textures\\container2_specular.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	diffuseTexture.setTextureType(SPECULAR_TEX);


	//light information
	StandardShader shader((path+"src\\shaders\\StandardShader.vs").c_str(), (path+"src\\shaders\\StandardShader.fs").c_str());
	shader.use();
	shader.setInt("PointNum", 1);
	shader.setInt("SpotNum", 1);
	shader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	shader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVector3("pointLights[0].position", glm::vec3(1.0f, 1.0f, -1.0f));
	shader.setVector3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVector3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.22f);
	shader.setFloat("pointLights[0].quadratic", 0.2f);
	shader.setVector3("spotLights[0].position", glm::vec3(1.0f, -1.0f, 1.0f));
	shader.setVector3("spotLights[0].direction", glm::vec3(-1.0f, 1.0f, -1.0f));
	shader.setVector3("spotLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("spotLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVector3("spotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("spotLights[0].innerCutoff", 0.9f);
	shader.setFloat("spotLights[0].outerCutOff", 0.5f);

	//load model
	Model nanosuit(path+"scene\\models\\nanosuit\\nanosuit.obj");

	// create cube map
	Skybox skybox(path + "scene\\materials\\textures\\skybox");

	

	//enable z buffer test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	//render loop
	while (!glfwWindowShouldClose(window.window)) {
		float currentFrame = glfwGetTime();
		camera.deltaTime = currentFrame - camera.lastFrame;
		camera.lastFrame = currentFrame;
		camera.processInput();
		camera.updateMatrixs();


		// render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		skybox.setCamera(&camera);
		skybox.drawSkyBox();

		glEnable(GL_DEPTH_TEST);
		shader.use();
		shader.setMatrix4("camera.view", camera.view);
		shader.setMatrix4("camera.projection", camera.projection);
		shader.setFloat("camera.near", camera.near);
		shader.setFloat("camera.far", camera.far);
		shader.setVector3("viewPos", camera.cameraPos);
		nanosuit.scale(glm::vec3(0.3f, 0.3f, 0.3f));
		nanosuit.drawModel(&shader);


		//check events
		glfwPollEvents();
		glfwSwapBuffers(window.window);


	}

	//release resources
	glfwTerminate();

	return 0;
}
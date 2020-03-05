#include "stdafx.h"
// content setting
#include "window.h"
#include "texture.h"

//rendering
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "frame.h"
#include "shadowMap.h"

//basic shape
#include "basic_shape/plane.h"

std::string path = "E:\\vs_workspace\\VanGL\\";

int main() {


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


	//uniform buffer
	unsigned int UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//use this function to allocate memory
	glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER,0);



	//shader
	// create cube map (need before model
	Skybox skybox(path + "scene\\materials\\textures\\skybox");
	Skybox blackSkybox(path + "scene\\materials\\textures\\blackSky");
	StandardShader postShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\renderShadowMap.fs").c_str());
	StandardShader planeShader((path + "src\\shaders\\basicShapeShader.vs").c_str(), (path + "src\\shaders\\basicShapeShader.fs").c_str());
	StandardShader shader((path + "src\\shaders\\StandardShader.vs").c_str(), (path + "src\\shaders\\StandardShader.fs").c_str()/*, (path + "src\\shaders\\geometry.gs").c_str()*/);
	shader.use();
	shader.setInt("PointNum", 0);
	shader.setInt("SpotNum", 0);
	shader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	shader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	//shader.setVector3("pointLights[0].position", glm::vec3(1.0f, 1.0f, -1.0f));
	//shader.setVector3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	//shader.setVector3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	//shader.setVector3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	//shader.setFloat("pointLights[0].constant", 1.0f);
	//shader.setFloat("pointLights[0].linear", 0.22f);
	//shader.setFloat("pointLights[0].quadratic", 0.2f);
	//shader.setVector3("spotLights[0].position", glm::vec3(0.0f, 1.0f, 0.0f));
	//shader.setVector3("spotLights[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
	//shader.setVector3("spotLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	//shader.setVector3("spotLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	//shader.setVector3("spotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	//shader.setFloat("spotLights[0].innerCutoff", 0.9f);
	//shader.setFloat("spotLights[0].outerCutOff", 0.5f);
	planeShader.use();
	planeShader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	planeShader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	planeShader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	planeShader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//get block index
	unsigned int matrixIndex = glGetUniformBlockIndex(shader.shaderProgramID, "Matrix");
	// bind matrix uniform to 2 binding point
	glUniformBlockBinding(shader.shaderProgramID, matrixIndex, BIND_POINT::MATRIX_POINT);
	unsigned int matrixIndex1 = glGetUniformBlockIndex(planeShader.shaderProgramID, "Matrix");
	glUniformBlockBinding(planeShader.shaderProgramID, matrixIndex1, BIND_POINT::MATRIX_POINT);
	//bind uniform buffer object to bind point
	glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, UBO);


	// use instance draw
	glm::mat4 matrix[500];
	int amount = 500;
	srand(time(NULL));
	// generate data
	for (int i = 0; i < amount; i++) {
		matrix[i] = glm::mat4(1.0);
		matrix[i] = glm::translate(matrix[i], glm::vec3((float)(rand() % 100 - 50), (float)(rand() % 100 - 50), (float)(rand() % 100 - 50)));
	}

	//enable z buffer test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//enable multisample
	glEnable(GL_MULTISAMPLE);
	
	//enable gamma correction (but some textures will be corrected twice, because they 
	// have been corrected already)
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//load model
	Model nanosuit(path + "scene\\models\\nanosuit_reflection\\nanosuit.obj");
	nanosuit.scale(glm::vec3(0.1f, 0.1f, 0.1f));
	nanosuit.translate(glm::vec3(0.0f, -5.0f, 0.0f));
	Model planet(path + "scene\\models\\planet\\planet.obj");

	//load plane
	Texture floor(path + "scene\\materials\\textures\\wood.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB);
	planeShader.use();
	planeShader.setInt("basicTex0", 0);
	Plane plane(floor.textureID);

	//shadow map 
	ShadowMap* shadowMap = new ShadowMap(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));

	//screen quad for post process
	Screen* screen= new Screen();

	//render loop
	while (!glfwWindowShouldClose(window.window)) {
		float currentFrame = glfwGetTime();
		camera.deltaTime = currentFrame - camera.lastFrame;
		camera.lastFrame = currentFrame;
		camera.processInput();
		camera.updateMatrixs();

		/*
		we can bind many uniform block to same bind point
		so we can use a same uniform buffer object to send data to
		shaders at the same time
		*/
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(camera.view));
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(camera.projection));
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &camera.near);
		glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &camera.far);
		glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, &shadowMap->lightSpace);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//shadow pass
		//generate shadow map
		shadowMap->bindBuffer(width,height);
		nanosuit.drawModel(shadowMap->depthShader, &blackSkybox);
		plane.Draw(shadowMap->depthShader);
		shadowMap->unBindBuffer();

		// render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shader.use();
		shader.setVector3("viewPos", camera.cameraPos);
		nanosuit.drawModel(&shader,&blackSkybox, shadowMap->depthTexture);
		//nanosuit.drawModelInstaced(&shader, &skybox, amount, matrix);
		planeShader.use();
		planeShader.setVector3("viewPos", camera.cameraPos);
		plane.Draw(&planeShader,shadowMap->depthTexture);

		//set the depth with 1 (so only draw on the pixels not cull bt object)
		glDepthFunc(GL_LEQUAL);
		blackSkybox.setCamera(&camera);
		blackSkybox.drawSkyBox();


		
		//screen->Draw(&postShader,shadowMap->depthTexture);

		//check events
		glfwPollEvents();
		glfwSwapBuffers(window.window);


	}

	//release resources
	glfwTerminate();

	return 0;
}

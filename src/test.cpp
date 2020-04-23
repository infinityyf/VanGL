#include "stdafx.h"
// content setting
#include "window.h"
#include "texture.h"

//rendering
#include "camera.h"
#include "model_loader/model.h"
#include "model_loader/PBRModel.h"
#include "shader.h"
#include "frame.h"
#include "shadowMap.h"
#include "AO/SSAO.h"

//basic shape
#include "basic_shape/plane.h"
#include "basic_shape/line.h"

//cube map
#include "light_probe/cubeMapGenerate.h"

//gui
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

std::string path = "E:\\vs_workspace\\VanGL\\";


//render setting
bool BLOOM_ENABLE = false;
bool DEFERRED_RENDERING = false;


int main() {

	int width = 800;
	int height = 600;
	Window window(width, height);
	if (window.window == NULL) return -1;
	//get cursor but make it invisible
	//window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.01f, 100.0f);
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
	glBindBuffer(GL_UNIFORM_BUFFER, 0);



	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseVariables.glsl").c_str());
	StandardShader shader((path + "src\\shaders\\StandardShader.vs").c_str(), (path + "src\\shaders\\StandardShader.fs").c_str()/*, (path + "src\\shaders\\geometry.gs").c_str()*/);
	StandardShader postShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\postProcessShader.fs").c_str());
	Skybox blackSkybox(path + "scene\\materials\\textures\\blackSky");


	

	//set light info
	shader.use();
	shader.setInt("PointNum", 0);
	shader.setInt("SpotNum", 0);
	shader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	shader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	
	//bind uniform buffer object to bind point
	glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, UBO);


	//get block index
	unsigned int matrixIndex = glGetUniformBlockIndex(shader.shaderProgramID, "Matrix");
	// bind matrix uniform to 2 binding point
	glUniformBlockBinding(shader.shaderProgramID, matrixIndex, BIND_POINT::MATRIX_POINT);



	
	//enable z buffer test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//enable multisample
	glEnable(GL_MULTISAMPLE);

	
	//load model
	Model nanosuit(path + "scene\\models\\nanosuit_reflection\\nanosuit.obj");
	nanosuit.scale(glm::vec3(0.1f, 0.1f, 0.1f));
	nanosuit.translate(glm::vec3(0.0f, -5.0f, 0.0f));


	//shadow map 
	ShadowMap* shadowMap = new ShadowMap(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(-1.0f, -1.0f, -1.0f));

	//screen quad for post process
	Screen* screen = new Screen();
	Frame* frame = new Frame(width, height);
	

	//set gui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


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
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &camera.nearPlane);
		glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &camera.farPlane);
		glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, glm::value_ptr(shadowMap->lightSpace));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//shadow pass
		shadowMap->bindBuffer(width, height);
		nanosuit.drawModel(shadowMap->depthShader, blackSkybox.skyBox);
		//plane.Draw(shadowMap->depthShader);
		shadowMap->unBindBuffer();

		// render to framebuffer
		frame->use();//render into defferent color buffer
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		nanosuit.drawModel(&shader, blackSkybox.skyBox, 0);


		//just render a screen quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		postShader.use();
		postShader.setInt("screenTexture", 0);
		//display color buffer
		screen->Draw(&postShader, frame->texAttachs[COLOR_TEXTURE]);
		

		//check events
		glfwPollEvents();


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("rendering setting");                     
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window.window);


	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}

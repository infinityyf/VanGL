//#include "stdafx.h"
//// content setting
//#include "window.h"
//#include "texture.h"
//
////rendering
//#include "camera.h"
//#include "shader.h"
//#include "shadowMap.h"
//#include "model_loader/model.h"
//
////basic shape
//#include "basic_shape/plane.h"
//#include "basic_shape/line.h"
//
////cube map
//#include "light_probe/cubeMapGenerate.h"
//
////gui
//#include "imgui.h"
//#include "imgui_impl_opengl3.h"
//#include "imgui_impl_glfw.h"
//
//
////render setting
//bool BLOOM_ENABLE = false;
//bool DEFERRED_RENDERING = false;
//std::string path = "E:\\vs_workspace\\VanGL\\";
//
//
//
//
//int main() {
//	int width = 800;
//	int height = 600;
//	Window window(width, height);
//	if (window.window == NULL) return -1;
//	//get cursor but make it invisible
//	//window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	Camera camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.01f, 100.0f);
//	camera.linkToWindow(&window);
//	//register size change call back
//	camera.SetFramebufferSizeCallback();
//	camera.SetCursorCallBack();
//	camera.SetScrollCallBack();
//
//	//uniform buffer
//	unsigned int UBO;
//	glGenBuffers(1, &UBO);
//	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
//	glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseLights.glsl").c_str());
//	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseFunctions.glsl").c_str());
//	StandardShader::createIncludeShaderFile((path + "src\\shaders\\includeShader\\baseVariables.glsl").c_str());
//	Skybox skybox(path + "scene\\materials\\textures\\skybox");
//
//	
//	StandardShader shader(
//		(path + "src\\shaders\\transparentShader\\transparent.vs").c_str()
//		, (path + "src\\shaders\\transparentShader\\transparent.fs").c_str());
//	
//	unsigned int matrixIndex = glGetUniformBlockIndex(shader.shaderProgramID, "Matrix");
//	glUniformBlockBinding(shader.shaderProgramID, matrixIndex, BIND_POINT::MATRIX_POINT);
//	glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, UBO);
//
//	Model dragon((path + "scene\\models\\dragon.obj"));
//
//	//rendering setting
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_MULTISAMPLE);
//
//
//	//gui setting
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	ImGui::StyleColorsDark();
//	// Setup Platform/Renderer bindings
//	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
//	ImGui_ImplOpenGL3_Init("#version 410");
//	float f0 = 0.04;
//
//	//render loop
//	while (!glfwWindowShouldClose(window.window)) {
//		float currentFrame = glfwGetTime();
//		camera.deltaTime = currentFrame - camera.lastFrame;
//		camera.lastFrame = currentFrame;
//		camera.processInput();
//		camera.updateMatrixs();
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
//		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(camera.view));
//		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(camera.projection));
//		glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &camera.nearPlane);
//		glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &camera.farPlane);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	
//
//		dragon.drawModel(&shader, skybox.skyBox, 0);
//
//		glDepthFunc(GL_LEQUAL);
//		skybox.drawSkyBox();
//
//
//		//check events
//		glfwPollEvents();
//		// Start the Dear ImGui frame
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//		ImGui::Begin("transparent setting");        
//		ImGui::SliderFloat("f0", &f0, 0.0f, 10.0f);
//		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//		ImGui::End();
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//		glfwSwapBuffers(window.window);
//
//
//	}
//	// Cleanup
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwTerminate();
//
//	return 0;
//}

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
#include "AO/SSAO.h"

//basic shape
#include "basic_shape/plane.h"
#include "basic_shape/line.h"


std::string path = "E:\\vs_workspace\\VanGL\\";

//render setting
bool BLOOM_ENABLE = false;
bool DEFERRED_RENDERING = true;


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
	//Skybox skybox(path + "scene\\materials\\textures\\skybox");
	Skybox blackSkybox(path + "scene\\materials\\textures\\blackSky");
	StandardShader debugShader((path + "src\\shaders\\debugShader\\debugShader.vs").c_str(), (path + "src\\shaders\\debugShader\\debugShader.fs").c_str());
	//show a picture
	StandardShader postShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\postProcessShader.fs").c_str());
	StandardShader planeShader((path + "src\\shaders\\basicShapeShader.vs").c_str(), (path + "src\\shaders\\basicShapeShader.fs").c_str());
	StandardShader shader((path + "src\\shaders\\StandardShader.vs").c_str(), (path + "src\\shaders\\StandardShader.fs").c_str()/*, (path + "src\\shaders\\geometry.gs").c_str()*/);
	//gbuffer rendering shader
	StandardShader gBuffer((path + "src\\shaders\\deferredRendering\\gBuffer.vs").c_str(), (path + "src\\shaders\\deferredRendering\\gBuffer.fs").c_str());
	StandardShader deferredRender((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\deferredRendering\\deferred.fs").c_str());
	StandardShader ssaoShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\ssao.fs").c_str());
	StandardShader ssaoBlurShader((path + "src\\shaders\\postProcess\\postProcessShader.vs").c_str(), (path + "src\\shaders\\postProcess\\ssaoBlur.fs").c_str());
	//set light info
	shader.use();
	shader.setInt("PointNum", 0);
	shader.setInt("SpotNum", 0);
	shader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	shader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	deferredRender.use();
	deferredRender.setInt("PointNum", 1);
	deferredRender.setInt("SpotNum", 0);
	deferredRender.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	deferredRender.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	deferredRender.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	deferredRender.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	deferredRender.setVector3("pointLights[0].position", glm::vec3(0.0f, 1.0f, 1.0f));
	deferredRender.setVector3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	deferredRender.setVector3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	deferredRender.setVector3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	deferredRender.setFloat("pointLights[0].constant", 1.0f);
	deferredRender.setFloat("pointLights[0].linear", 0.22f);
	deferredRender.setFloat("pointLights[0].quadratic", 0.2f);
	deferredRender.setInt("g_Position",POSITION_TEXTURE);
	deferredRender.setInt("g_Normal", NORMAL_TEXTURE);
	deferredRender.setInt("g_Ambient", AMBIENT_TEXTURE);
	deferredRender.setInt("g_Specular", SPECULAR_TEXTURE);
	deferredRender.setInt("g_Diffuse", DIFFUSE_TEXTURE);
	deferredRender.setInt("g_SSAO", 7);
	planeShader.use();
	planeShader.setVector3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	planeShader.setVector3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	planeShader.setVector3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	planeShader.setVector3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	ssaoShader.use();
	ssaoShader.setInt("width", width);
	ssaoShader.setInt("height", height);

	//get block index
	unsigned int matrixIndex = glGetUniformBlockIndex(shader.shaderProgramID, "Matrix");
	// bind matrix uniform to 2 binding point
	glUniformBlockBinding(shader.shaderProgramID, matrixIndex, BIND_POINT::MATRIX_POINT);
	unsigned int matrixIndex1 = glGetUniformBlockIndex(planeShader.shaderProgramID, "Matrix");
	glUniformBlockBinding(planeShader.shaderProgramID, matrixIndex1, BIND_POINT::MATRIX_POINT);
	unsigned int matrixIndex2 = glGetUniformBlockIndex(debugShader.shaderProgramID, "Matrix");
	glUniformBlockBinding(debugShader.shaderProgramID, matrixIndex2, BIND_POINT::MATRIX_POINT);
	unsigned int matrixIndex3 = glGetUniformBlockIndex(gBuffer.shaderProgramID, "Matrix");
	glUniformBlockBinding(gBuffer.shaderProgramID, matrixIndex3, BIND_POINT::MATRIX_POINT);
	unsigned int samplerIndex1 = glGetUniformBlockIndex(ssaoShader.shaderProgramID, "Samples");
	glUniformBlockBinding(ssaoShader.shaderProgramID, samplerIndex1, BIND_POINT::SSAO_SAMPLER_POINT);
	//bind uniform buffer object to bind point
	glBindBufferBase(GL_UNIFORM_BUFFER, BIND_POINT::MATRIX_POINT, UBO);


	// use instance draw
	//glm::mat4 matrix[500];
	//int amount = 500;
	//srand(time(NULL));
	//// generate data
	//for (int i = 0; i < amount; i++) {
	//	matrix[i] = glm::mat4(1.0);
	//	matrix[i] = glm::translate(matrix[i], glm::vec3((float)(rand() % 100 - 50), (float)(rand() % 100 - 50), (float)(rand() % 100 - 50)));
	//}

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

	//load plane
	Texture floor(path + "scene\\materials\\textures\\wood.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB);
	planeShader.use();
	planeShader.setInt("basicTex0", 0);
	Plane plane(floor.textureID);

	//shadow map 
	ShadowMap* shadowMap = new ShadowMap(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(-1.0f, -1.0f, -1.0f));

	//screen quad for post process
	Screen* screen= new Screen();
	Frame* frame = new Frame(width,height);
	SSAO* ssao = new SSAO(width, height);

	glBindBuffer(GL_UNIFORM_BUFFER, ssao->ssaoUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 1024, glm::value_ptr(ssao->ssaoKernel[0]));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


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
		shadowMap->bindBuffer(width,height);
		nanosuit.drawModel(shadowMap->depthShader, &blackSkybox);
		plane.Draw(shadowMap->depthShader);
		shadowMap->unBindBuffer();

		// render to framebuffer
		frame->use();//render into defferent color buffer
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//deferred rendering
		if (DEFERRED_RENDERING) {
			nanosuit.drawModel(&gBuffer, &blackSkybox);

			//apply ssao
			glBindFramebuffer(GL_FRAMEBUFFER, ssao->ssaoFBO);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ssaoShader.use();
			ssaoShader.setMatrix4("projection", camera.projection);
			ssaoShader.setMatrix4("view", camera.view);
			screen->DrawSSAO(&ssaoShader, frame->texAttachs[POSITION_TEXTURE], frame->texAttachs[NORMAL_TEXTURE],ssao->noiseTexture);

			//apply ssao blur
			glBindFramebuffer(GL_FRAMEBUFFER, ssao->ssaoBlurFBO);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			screen->DrawSSAOBlur(&ssaoBlurShader, ssao->ssaoBlurColorBuffer);

			//just render a screen quad(for display)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			////display a specifical poicture
			//postShader.use();
			//postShader.setInt("screenTexture", 0);
			////display mrt
			//glViewport(0, 0, width / 2, height / 2);
			//screen->Draw(&postShader, frame->texAttachs[POSITION_TEXTURE]);
			//glViewport(width / 2, height / 2, width / 2, height / 2);
			//screen->Draw(&postShader, frame->texAttachs[NORMAL_TEXTURE]);
			//glViewport(width / 2, 0, width / 2, height / 2);
			//screen->Draw(&postShader, frame->texAttachs[AMBIENT_TEXTURE]);
			//glViewport(0, height / 2, width / 2, height / 2);
			//screen->Draw(&postShader, frame->texAttachs[SPECULAR_TEXTURE]);
			/*postShader.use();
			postShader.setInt("screenTexture", 0);
			screen->Draw(&postShader, ssao->ssaoColorBuffer);*/

			deferredRender.use();
			deferredRender.setVector3("viewPos", camera.cameraPos);
			screen->DeferredRender(&deferredRender, frame->texAttachs, ssao->ssaoBlurColorBuffer);
		}
		//forward rendering
		else
		{
			shader.use();
			shader.setVector3("viewPos", camera.cameraPos);
			nanosuit.drawModel(&shader, &blackSkybox, shadowMap->depthTexture);
			//nanosuit.drawModelInstaced(&shader, &skybox, amount, matrix);
			planeShader.use();
			planeShader.setVector3("viewPos", camera.cameraPos);
			plane.Draw(&planeShader, shadowMap->depthTexture);


			//set the depth with 1 (so only draw on the pixels not cull bt object)
			glDepthFunc(GL_LEQUAL);
			blackSkybox.setCamera(&camera);
			blackSkybox.drawSkyBox();

			//just render a screen quad
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			postShader.use();
			postShader.setInt("screenTexture", 0);
			//display color buffer
			screen->Draw(&postShader, frame->texAttachs[COLOR_TEXTURE]);
		}

		//check events
		glfwPollEvents();
		glfwSwapBuffers(window.window);


	}
	//stop haptic
	//StopHapticLoopPhantom();

	//release resources
	glfwTerminate();

	return 0;
}

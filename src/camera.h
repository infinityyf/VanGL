#pragma once
#ifndef CAMERA
#define CAMERA

#include "window.h"


class Camera{
public:
	//contain window
	Window* window;
public:
	//camera position info
	static glm::vec3 cameraPos;
	static glm::vec3 cameraFront;
	static glm::vec3 cameraUp;

	//rotation information
	//curse position
	static double lastx;
	static double lasty;
	//view euler angle
	static double yaw;
	static double pitch;
	//fov
	static double fov;
	//
	static bool firstMouse;

	//time related
	float deltaTime = 0.0f; // time between current and last frame
	float lastFrame = 0.0f; // last frame time

	//coordinate translate matrixs
	glm::mat4 view;
	glm::mat4 projection;

public:
	Camera(glm::vec3, glm::vec3, glm::vec3);
	void linkToWindow(Window* window);
	//render function
	void renderScene();

	//call back function
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void cursor_position_callback(GLFWwindow* window, double posx, double posy);
	static void scroll_fov_callback(GLFWwindow* window, double xoffset, double yoffset);
	//set mouse call back
	void SetFramebufferSizeCallback();
	void SetCursorCallBack();
	void SetScrollCallBack();
	//get key input process
	void processInput();

	//update matrixs
	void updateMatrixs();
};

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) {
	cameraPos = pos;
	cameraFront = front;
	cameraUp = up;

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
}

inline void Camera::linkToWindow(Window* window)
{
	this->window = window;
	lastx = window->windowWidth / 2;
	lasty = window->windowHeight / 2;
}

inline void Camera::renderScene()
{
	//1. update camera matrix
	//2. for all models set matrixs into shaders
	//3. for all models set textures
	//4. render

	//extention
	//1.render path(forward? deferred?)
	//2.shadow
	//3.global illumination
	//4.voxel rendering
}


inline void Camera::SetFramebufferSizeCallback()
{
	glfwSetFramebufferSizeCallback(window->window, framebuffer_size_callback);
}

inline void Camera::SetCursorCallBack()
{
	glfwSetCursorPosCallback(window->window, cursor_position_callback);
}

inline void Camera::SetScrollCallBack()
{
	glfwSetScrollCallback(window->window, scroll_fov_callback);
}



//callback functions
void Camera::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Camera::cursor_position_callback(GLFWwindow* window, double posx, double posy) {
	if (firstMouse)
	{
		lastx = posx;
		lasty = posy;
		firstMouse = false;
	}
	//calculate delate between current cursor position and last position
	double xoffset = posx - lastx;
	double yoffset = lasty - posy;
	lastx = posx;
	lasty = posy;

	double sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front = glm::vec3(1.0f);
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}
void Camera::scroll_fov_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void Camera::processInput() {
	//get key press event
	if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window->window, true);
	// adjust accordingly if use more time to render then move faster
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window->window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += glm::normalize(cameraUp) * cameraSpeed;
	if (glfwGetKey(window->window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= glm::normalize(cameraUp) * cameraSpeed;
}

inline void Camera::updateMatrixs()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective((float)glm::radians(fov), (float)window->windowWidth / window->windowHeight, 0.1f, 100.0f);
}

glm::vec3 Camera::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Camera::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
double Camera::lastx = 0;
double Camera::lasty = 0;
double Camera::yaw = -90.0f;
double Camera::pitch = 0;
double Camera::fov = 45.0f;
bool Camera::firstMouse = true;
#endif
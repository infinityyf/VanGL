#pragma once
#ifndef WINDOW
#define WINDOW

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

enum WIN_ERROR
{
	WIN_CREATE_FAILE = -1,
	GLAD_LOAD_FAILE = -1
};

typedef void (*size_callback)(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}



class Window {
public:
	int windowWidth;
	int windowHeight;
	GLFWwindow* window;
	Window(const int width, const int height);
	
	void SetFramebufferSizeCallback(size_callback function);
};

Window::Window(const int width, const int height)
{
	//init glfw
	glfwInit();
	//configure glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//only use core mode (donnt use extend function)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window
	window = glfwCreateWindow(width, height, "VANGL", NULL, NULL);
	if (window == NULL) {
		std::cerr << "fail to create window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	//init glad (glfwGetProcAddress is address of opengl function pointers)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "fail to init glad" << std::endl;
		return;
	}

	/*
	set view port
	the size can be smaller than window
	can use callback function to resize view port
	transform coordinate from NDC to screen
	*/
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;
}
inline void Window::SetFramebufferSizeCallback(size_callback framebuffer_size_callback)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
#endif

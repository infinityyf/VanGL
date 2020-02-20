#pragma once
#ifndef WINDOW
#define WINDOW

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm.hpp>
// matrix operation
#include <gtc/matrix_transform.hpp>
// send data pointer
#include <gtc/type_ptr.hpp>

enum WIN_ERROR
{
	WIN_CREATE_FAILE = -1,
	GLAD_LOAD_FAILE = -1
};

//add limitation of class name
//this is the way to define the function pointer of class function
//but we cannot use it without object , unless it is a static function(we can find its address without an object)
//typedef void (*size_callback)(GLFWwindow*, int, int);
//typedef void (*cursor_callback)(GLFWwindow*, double, double);
//typedef void (*scroll_callback)(GLFWwindow*, double, double);
//size_callback = &Window::framebuffer_size_callback


class Window {
public:

	int windowWidth;
	int windowHeight;
	GLFWwindow* window;
	Window(const int width, const int height);
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	//void cursor_position_callback(GLFWwindow* window, double posx, double posy);
	//void scroll_fov_callback(GLFWwindow* window, double xoffset, double yoffset);
public:
	void SetInputMode(int mode, int value);
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
// deal with cursor
inline void Window::SetInputMode(int mode, int value)
{
	glfwSetInputMode(window, mode, value);
}


#endif

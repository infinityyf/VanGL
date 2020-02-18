#pragma once
#ifndef CONTEXT_MANAFER
#define CONTEXT_MANAFER

#include "stdafx.h"

int InitGLFWandGLAD(GLFWwindow** window, int width, int height) {

	//init glfw
	glfwInit();
	//configure glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//only use core mode (donnt use extend function)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window
	*window = glfwCreateWindow(width, height, "VANGL", NULL, NULL);
	if (window == NULL) {
		cout << "fail to create window" << endl;
		glfwTerminate();
		return WIN_CREATE_FAILE;
	}
	glfwMakeContextCurrent(*window);

	//init glad (glfwGetProcAddress is address of opengl function pointers)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "fail to init glad" << endl;
		return GLAD_LOAD_FAILE;
	}

	/*
	set view port
	the size can be smaller than window
	can use callback function to resize view port
	transform coordinate from NDC to screen
	*/
	glViewport(0, 0, width, height);
	return 0;
}

#endif
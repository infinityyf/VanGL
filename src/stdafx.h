#pragma once
#ifndef STDAFX
#define STDAFX             
#endif

#include <iostream>
using namespace std;
//easy to use opengl API. it needs to be included before glfw
//manage function pointers of openGL
#include <glad/glad.h>

//create opengl context, define windows and deal input
#include <GLFW/glfw3.h>


const int WIN_CREATE_FAILE = -1;
const int GLAD_LOAD_FAILE = -1;
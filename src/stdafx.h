#pragma once
#ifndef STDAFX
#define STDAFX             
#endif

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;
//easy to use opengl API. it needs to be included before glfw
//manage function pointers of openGL
#include <glad/glad.h>

//create opengl context, define windows and deal input
#include <GLFW/glfw3.h>


#include <glm.hpp>
// matrix operation
#include <gtc/matrix_transform.hpp>
// send data pointer
#include <gtc/type_ptr.hpp>


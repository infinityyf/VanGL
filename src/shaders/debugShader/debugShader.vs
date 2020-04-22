#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0)in vec3 aPosition;

void main(){
    gl_Position = camera.projection* camera.view* model * vec4(aPosition,1.0);
}
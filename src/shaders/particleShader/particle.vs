#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0) in vec3 aPos; 

void main()
{
    gl_Position = camera.projection * camera.view * vec4(aPos, 1.0);
}
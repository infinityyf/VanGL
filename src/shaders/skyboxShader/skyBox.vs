#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

void main()
{
    //TexCoords = vec3(-aPos.x,-aPos.y,-aPos.z);
    TexCoords = aPos;
    vec4 pos  = camera.projection * mat4(mat3(camera.view)) * vec4(aPos, 1.0);
    // coord beform NDC so it will be divide by w
    gl_Position = pos.xyzw;
}
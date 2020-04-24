#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCoord;


out VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
}vs_out;


void main()
{
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    vs_out.coord = aCoord;
    vs_out.normal = aNormal;
    //world coord
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragLightSpacePos = lightSpaceMatrix * vec4(vs_out.FragPos,1.0f);
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location=2) in vec2 aCoord;


out VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
}vs_out;

struct Camera{          //align
    mat4 view;          //64    //0
    mat4 projection;    //64    //64
    float near;         //4     //128
    float far;          //4     //132
};                              //144
    

//use uniform block we can access component directly nont need to use Matrix
layout(std140) uniform Matrix{
    Camera camera;      //144   //0
    //mat4 model;         //64    //208
    mat4 lightSpaceMatrix;
};

uniform mat4 model;

void main()
{
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    vs_out.coord = aCoord;
    vs_out.normal = aNormal;
    //world coord
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragLightSpacePos = lightSpaceMatrix * vec4(vs_out.FragPos,1.0f);
}
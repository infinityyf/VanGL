#version 430 core


layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTex; 
out vec2 texCoord;
struct Camera{          //align
    mat4 view;          //64    //0
    mat4 projection;    //64    //64
    float near;         //4     //128
    float far;          //4     //132
};                              //144
layout(std140) uniform Matrix{
    Camera camera;      //144   //0
    mat4 lightSpaceMatrix;//64    //208
};
uniform mat4 model;
void main()
{
    gl_Position = camera.projection * camera.view *model* vec4(aPos, 1.0);
    texCoord = aTex;
}
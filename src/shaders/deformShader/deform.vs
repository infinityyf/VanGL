#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTex; 
out vec2 texCoord;
struct Camera{          //align
    mat4 view;          //64    //0
    mat4 projection;    //64    //64
    float near;         //4     //128
    float far;          //4     //132
};                              //144
layout(std140) uniform Matrix{
    Camera camera;      //144   //0
};
void main()
{
    gl_Position = camera.projection * camera.view * vec4(aPos, 1.0);
    texCoord = aTex;
}
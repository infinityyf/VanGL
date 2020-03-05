#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT{
    vec3 Normal;
}vs_out;

struct Camera{          //align
    mat4 view;          //64    //0
    mat4 projection;    //64    //64
    float near;         //4     //128
    float far;          //4     //132
};   

//use uniform block we can access component directly nont need to use Matrix
layout(std140) uniform Matrix{
    Camera camera;      //144   //0
    //mat4 model;         //64    //208
    mat4 lightSpaceMatrix;
};

uniform mat4 model;

void main(){
    gl_Position = camera.projection * camera.view * model * vec4(position, 1.0);
    vs_out.Normal = normal;
}
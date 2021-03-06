#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;   //use draw instance update every instance

out VS_OUT{
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
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
    gl_Position = camera.projection * camera.view * model * instanceMatrix * vec4(aPos, 1.0);
    vs_out.TexCoord = aTexCoord;
    //world coord
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = mat3(transpose(inverse(model)))*aNormal;
    vs_out.Normal = aNormal;
    vs_out.View = camera.view;
    vs_out.Projection = camera.projection;
    vs_out.Near = camera.near;
    vs_out.Far = camera.far;
}
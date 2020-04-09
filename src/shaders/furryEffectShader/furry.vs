#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;


out VS_OUT{
    vec3 bitangent;
    vec3 normal;
    vec3 tangent;
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
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(camera.view * model)));
    vec3 N = normalize(vec3(camera.projection * vec4(normalMatrix * aNormal, 0.0)));
    vec3 T =normalize(vec3(camera.projection * vec4(normalMatrix * aTangent, 0.0)));
    vs_out.tangent = T;
    vs_out.normal = N;
    vs_out.bitangent = normalize(cross(N,T)); 
}
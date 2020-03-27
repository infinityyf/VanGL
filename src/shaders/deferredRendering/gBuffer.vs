#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out VS_OUT{
    mat3 TBN;
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    float near;
    float far;
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
    mat4 lightSpaceMatrix;//64    //208
};

uniform mat4 model;




void main()
{
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    vs_out.TexCoord = aTexCoord;
    //world coord
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    //calculate TBN matrix
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(T,N));    //left hand
    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = TBN;
    vs_out.Normal = aNormal;
    vs_out.near = camera.near;
    vs_out.far = camera.far;
}
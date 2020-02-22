#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out mat4 View;
out mat4 Projection;
out float Near;
out float Far;

struct Camera{
    mat4 view;
    mat4 projection;
    float near;
    float far;
};
uniform Camera camera;

void main()
{
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    //world coord
    FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = mat3(transpose(inverse(model)))*aNormal;
    Normal = aNormal;
    View = camera.view;
    Projection = camera.projection;
    Near = camera.near;
    Far = camera.far;
}
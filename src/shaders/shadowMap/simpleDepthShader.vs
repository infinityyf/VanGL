#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 lightMatrix;
uniform mat4 model;

void main(){
    gl_Position = lightMatrix * model * vec4(aPosition, 1.0f);
}
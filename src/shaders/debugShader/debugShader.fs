#version 410 core
out vec4 FragColor;
in vec3 color;
in vec3 Normal_FS_in;
void main(){
    FragColor = vec4(color,1.0f);
}
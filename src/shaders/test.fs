#version 330 core
layout (location = 0) out vec4 FragColor;



in VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
    vec3 color;
}fs_in;

uniform vec3 lightPos;
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
uniform vec3 viewPos;

void main()
{
    vec3 viewDir = normalize(viewPos -fs_in.FragPos);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir,fs_in.normal);
    vec3 color = vec3(1.0,0.0,0.0) * Phong(viewDir,reflectDir,8.0f);
    FragColor = vec4(color,1.0f);
}

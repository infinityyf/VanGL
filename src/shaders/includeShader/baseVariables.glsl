//pbr setting
uniform float brightness;
uniform float roughness;
uniform float metal;
uniform float aoScale;

//matrix date
struct Camera{          
    mat4 view;         
    mat4 projection;    
    float near;        
    float far;         
};                             
layout(std140) uniform Matrix{
    Camera camera;     
    mat4 lightSpaceMatrix;
};
uniform mat4 model;
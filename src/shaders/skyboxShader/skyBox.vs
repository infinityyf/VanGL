#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

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

void main()
{
    //TexCoords = vec3(-aPos.x,-aPos.y,-aPos.z);
    TexCoords = aPos;
    vec4 pos  = camera.projection * mat4(mat3(camera.view)) * vec4(aPos, 1.0);
    // coord beform NDC so it will be divide by w
    gl_Position = pos.xyzw;
}
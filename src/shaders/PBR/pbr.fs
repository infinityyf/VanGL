#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

layout (location = 0) out vec4 FragColor;


in VS_OUT{
    mat3 TBN;
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
    vec4 FragLightSpacePos; //only support one light shadow
}fs_in;


//material
struct StandardMaterial{
    samplerCube sky;
    sampler2D albedo;
    sampler2D metallic;
    sampler2D rougnness;
    sampler2D ao;
};
uniform vec3 viewPos;


void main()
{

    FragColor = vec4(result,1.0f);
}

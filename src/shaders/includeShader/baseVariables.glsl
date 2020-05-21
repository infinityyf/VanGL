//matrix data
struct Camera{          
    mat4 view;         
    mat4 projection;    
    float near;        
    float far;         
};                             
layout(std140) uniform Matrix{
    Camera camera;     
    mat4 lightSpaceMatrix;
    vec3 viewPos;       //16    //224
};
uniform mat4 model;

//forward standard material
struct StandardMaterial{
    samplerCube sky;
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};
uniform StandardMaterial material;

//shadow map
uniform sampler2D shadowMap;


//pbr material
struct pbrMaterial{
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D rougnnessMap;
    sampler2D aoMap;
};
uniform pbrMaterial PBRmaterial;
//预过滤贴图
uniform samplerCube prefilterMap;
uniform samplerCube irradianceMap;
uniform sampler2D brdfMap;
//pbr setting
uniform float brightness;
uniform float roughness;
uniform float metal;
uniform float aoScale;
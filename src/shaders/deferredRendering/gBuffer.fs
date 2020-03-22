#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

layout (location = 2) out vec4 FragPosition;
layout (location = 3) out vec4 FragNormal;
layout (location = 4) out vec4 FragAlbedo;
layout (location = 5) out vec4 FragSpecular;


in VS_OUT{
    mat3 TBN;
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
}fs_in;

//material
struct StandardMaterial{
    samplerCube sky;
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};
uniform StandardMaterial material;

void main()
{
//use normal map
    vec3 normal = texture(material.normal,fs_in.TexCoord).rgb;
    normal = normalize(normal*2.0f - 1.0f);// change into vector not color
    normal = normalize(fs_in.TBN * normal);// change normal from tangent coord to world coord

    //write to g buffer
    FragNormal = vec4(normal,1.0);
    FragPosition =vec4(fs_in.FragPos,1.0f);
    FragAlbedo = texture(material.ambient,fs_in.TexCoord);
    FragSpecular = texture(material.specular,fs_in.TexCoord);
}

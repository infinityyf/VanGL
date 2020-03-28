#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4


layout (location = 2) out vec4 FragPosition;
layout (location = 3) out vec4 FragNormal;
layout (location = 4) out vec4 FragAmbient;
layout (location = 5) out vec4 FragSpecular;
layout (location = 6) out vec4 FragDiffuse;


in VS_OUT{
    mat3 TBN;
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    float near;
    float far;
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

float depthToEyeCoord(float ScreenDepth){
    float NDCDepth = ScreenDepth * 2 - 1;//from screen to NDC
    float depth = (2.0 * fs_in.near * fs_in.far) / (fs_in.far + fs_in.near - NDCDepth * (fs_in.far - fs_in.near));   //from NDC to linear space
    return depth; // to 01
}

void main()
{
//use normal map
    vec3 normal = texture(material.normal,fs_in.TexCoord).rgb;
    normal = normalize(normal*2.0f - 1.0f);// change into vector not color
    normal = normalize(fs_in.TBN * normal);// change normal from tangent coord to world coord
    normal = (normal+1.0f)/2.0f;
    float fragDepth = depthToEyeCoord(gl_FragCoord.z);
    //write to g buffer
    FragNormal = vec4(normal,1.0);
    FragPosition =vec4(fs_in.FragPos,fragDepth);
    FragAmbient = texture(material.ambient,fs_in.TexCoord);
    FragSpecular = texture(material.specular,fs_in.TexCoord);
    FragDiffuse = texture(material.diffuse,fs_in.TexCoord);
}

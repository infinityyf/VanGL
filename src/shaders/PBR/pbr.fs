#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

layout (location = 0) out vec4 FragColor;


in VS_OUT{
    mat3 TBN;
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
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D rougnnessMap;
    sampler2D aoMap;
};
uniform StandardMaterial PBRmaterial;
uniform vec3 viewPos;

//F
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  
//D
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;  //have different form

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
//G
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{

    vec3 albedo = texture(PBRmaterial.albedoMap,fs_in.TexCoord).rgb;
    albedo  = pow(albedo, 2.2); //translate into linear space

    vec3 normal = texture(PBRmaterial.normalMap,fs_in.TexCoord).rgb;
    normal = normalize(normal*2.0f - 1.0f);
    normal = normalize(fs_in.TBN * normal);

    float metallic = texture(PBRmaterial.metallicMap,fs_in.TexCoord).r;

    float roughness = texture(PBRmaterial.rougnnessMap,fs_in.TexCoord).r;

    float ao = texture(PBRmaterial.aoMap,fs_in.TexCoord).r;



    FragColor = vec4(result,1.0f);
}

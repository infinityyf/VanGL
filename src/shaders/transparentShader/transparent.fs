#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
#include "/baseLights.glsl"
#include "/baseFunctions.glsl"
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
    vec4 FragLightSpacePos;
}fs_in;

uniform float f0;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

void main()
{

    


    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 refl = reflect(-viewDir,fs_in.Normal);
    float costheta = dot(viewDir,fs_in.Normal);
    vec3 refr = refract(-viewDir,fs_in.Normal,0.5f);

    vec3 F0 = vec3(f0); 
    vec3 k = fresnelSchlick(costheta, F0);
    vec3 colorl = texture(material.sky,refl).rgb;
    //vec3 colorr = texture(material.sky,refr).rgb * (1-k)*0.1;
    FragColor = vec4(colorl,1.0f);
}
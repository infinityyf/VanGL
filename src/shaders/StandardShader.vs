#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out VS_OUT{
    mat3 TBN;
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
    vec4 FragLightSpacePos;
}vs_out;

out gl_PerVertex{
    vec4 gl_Position;
    float gl_PointSize;
};


void main()
{
    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
    vs_out.Normal = aNormal;
    vs_out.TexCoord = aTexCoord;
    //world coord
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.View = camera.view;
    vs_out.Projection = camera.projection;
    vs_out.Near = camera.near;
    vs_out.Far = camera.far;
    vs_out.FragLightSpacePos = lightSpaceMatrix * vec4(vs_out.FragPos,1.0f);

    //calculate TBN matrix
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N,T));    //left hand
    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = TBN;
    
}
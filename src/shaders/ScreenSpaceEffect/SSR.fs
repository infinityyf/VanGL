#version 430 core
#extension GL_ARB_shading_language_include : require
#include "/baseVariables.glsl"
layout (location = 0) out vec4 FragColor;



in VS_OUT{
    vec2 coord;//纹理坐标
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
}fs_in;

uniform sampler2D screenDepth;
uniform sampler2D screenColor;

void main()
{
    //最终的颜色
    vec4 color;
    //光线步进的长度
    float delta = 0.001f;
    //根据像素位置计算反射光线
    vec4 currentSamplePosition = camera.view * vec4(fs_in.FragPos,1.0);
    vec3 rayDir =normalize(currentSamplePosition.xyz);
    vec4 screenNormal = normalize(camera.view * vec4(fs_in.normal,1.0));
    rayDir = reflect(rayDir,screenNormal.xyz);
    //不断步进计算和屏幕空间深度值的相交
    //计算采样点在视点坐标系中的坐标
    for(int i=0;i<100;i++){
        currentSamplePosition.xyz += rayDir*delta;
        //转化到屏幕空间(获取纹理的坐标)
        vec4 screenPos = camera.projection * currentSamplePosition;
        float depth = texture(screenDepth,screenPos.xy/screenPos.w).z;
        if(depth<currentSamplePosition.z){
            //获取当前位置的颜色，作为反射
            color = texture(screenColor,screenPos.xy/screenPos.w);
            break;
        }
    }
    FragColor = color;
}

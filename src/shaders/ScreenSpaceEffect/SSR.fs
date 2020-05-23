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
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    bool hit = true;
    //光线步进的长度
    float delta = 0.1f;
    //根据像素位置计算反射光线(视点坐标中)
    vec4 currentSamplePosition = camera.view * vec4(fs_in.FragPos,1.0);
    vec3 rayDir =normalize(currentSamplePosition.xyz);
    vec3 screenNormal = normalize(mat3(camera.view) * fs_in.normal);
    rayDir = reflect(rayDir,screenNormal);
    //不断步进计算和屏幕空间深度值的相交
    //计算采样点在视点坐标系中的坐标
    for(int i=0;i<1000;i++){
        currentSamplePosition.xyz += rayDir*delta;
        //转化到屏幕空间(获取纹理的坐标)
        vec4 screenPos = camera.projection * currentSamplePosition;
        screenPos /= screenPos.w; 
        screenPos.xy = screenPos.xy*0.5+0.5;
        //超过屏幕的部分就不用计算了
        if(screenPos.x > 1.0 || screenPos.x < 0.0 ) break;
        if(screenPos.y > 1.0 || screenPos.y < 0.0 ) break;
        float depth = texture(screenDepth,screenPos.xy).b;
        if(depth>currentSamplePosition.z){
            //获取当前位置的颜色，作为反射
            //color = vec4(0.0,0.0,-1*depth,1.0);
            color = texture(screenColor,screenPos.xy);
            break;
        }
        // if(depth < 0){
        //     color = vec4(0.0,0.0,-1*depth,1.0);
        //     break;
        // }
    }
    FragColor = color;
}

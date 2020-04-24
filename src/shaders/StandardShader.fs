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

float PCFShadowSmooth(vec3 NDCCoord,vec3 normal){
    // plane is not like mesh
    // even it has to face it cannot avoid shadow scne, there is not distance between front and back
    float bias = max(0.005 * (1.0 - dot(normal, normalize(-dirLight.direction))), 0.00005);
    float shadow = 0.0;
    vec2 size = 1/textureSize(shadowMap,0);
    for(int i=-1;i<1;i++){
        for(int j=-1;j<1;j++){
            float closestDepth = texture(shadowMap,NDCCoord.xy+vec2(i,j)*size).r;    // * is different from dot
            shadow += (NDCCoord.z - bias)>closestDepth? 1:0;
        }
    }
    shadow /= 9.0f;
    return shadow;
}
//calculate shadow
float ShadowMap(vec3 normal,vec4 fragLightSpacePos){
    //FragLightSpacePos is in NDC but it doesnt divide by w
    vec4 NDCCoord = fragLightSpacePos/fragLightSpacePos.w;
    //this coord is frag coord in light space(between -1 and 1),but depthtex is between 0,1
    // so as UV coord
    NDCCoord = NDCCoord*0.5 + 0.5;
    if(NDCCoord.z>1.0) return 0.0f;
    float shadow = PCFShadowSmooth(NDCCoord.xyz,normal);
    return shadow;
}

void main()
{
    //use normal map
    vec3 normal = texture(material.normal,fs_in.TexCoord).rgb;
    normal = normalize(normal*2.0f - 1.0f);// change into vector not color
    normal = normalize(fs_in.TBN * normal);// change normal from tangent coord to world coord
    //normal = fs_in.Normal;

    float shadow = 0;
    shadow = ShadowMap(normal,fs_in.FragLightSpacePos);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 refDir = reflect(-viewDir,normal);
    //calculate the reflect light
    vec3 reflectColor = texture(material.ambient,fs_in.TexCoord).rgb * texture(material.sky,refDir).rgb;
    //calculate all lights
    vec3 point = vec3(0.0f);
    for(int i=0;i<min(PointNum,NUM_POINT_LIGHTS);i++){
        point += calculatePointLight(pointLights[i],normal,viewDir,material,shadow,fs_in.FragPos,fs_in.TexCoord);
    }
    vec3 spot = vec3(0.0f);
    for(int i=0;i<min(SpotNum,NUM_SPOT_LIGHTS);i++){
        spot += calculateSpotLight(spotLights[i],normal,viewDir,material,shadow,fs_in.FragPos,fs_in.TexCoord);
    }
    vec3 direct = vec3(0.0f);
    direct += calculateDirectLight(dirLight,normal,viewDir,material,shadow,fs_in.TexCoord);

    vec3 result = spot + point + direct + reflectColor;

    //FragColor = vec4(result,1.0f);
    FragColor = vec4(1.0,1.0,1.0,1.0f);
}

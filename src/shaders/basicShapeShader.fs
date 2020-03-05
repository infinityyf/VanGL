#version 330 core
in VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
}fs_in;
out vec4 FragColor;  

uniform sampler2D basicTex0;
uniform vec3 viewPos;

//spot light
// struct SpotLight{
//     vec3 position;
//     vec3 direction;
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
//     //cosine of in/out angle
//     float innerCutoff;
//     float outerCutOff;
// };
//direct light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform sampler2D shadowMap;
uniform DirLight dirLight;
// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//spotlight calculation
// vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir){
//     vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
//     //diffuse
//     float diff = max(dot(lightDir,Normal),0.0f);
//     vec3 diffuse = diff * spotLight.diffuse * texture(basicTex0,fs_in.coord).rgb;

//     //specular
//     vec3 reflectDir = reflect(-lightDir,Normal);
//     //float spec = Phong(viewDir,reflectDir,32.0f);
//     float spec = Blinn_Phong(viewDir,lightDir,Normal,8.0f);
//     vec3 specular = spec * spotLight.specular * texture(basicTex0,fs_in.coord).rgb;

//     //ambient
//     vec3 ambient = spotLight.ambient * texture(basicTex0,fs_in.coord).rgb;

//     //attenuation
//     float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
//     float delta = spotLight.innerCutoff - spotLight.outerCutOff;
//     float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
//     vec3 result = diffuse + specular + ambient;
//     result *= attenuation;
//     return result;    
// }

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,float shadow){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * texture(basicTex0,fs_in.coord).rgb;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    //float spec = Phong(viewDir,reflectDir,32.0f);
    float spec = Blinn_Phong(viewDir,lightDir,Normal,8.0f);
    vec3 specular = spec * dirLight.specular * texture(basicTex0,fs_in.coord).rgb;

    //ambient
    vec3 ambient = dirLight.ambient * texture(basicTex0,fs_in.coord).rgb;
    return (diffuse + specular)*(1.0f - shadow) + ambient;
}


//calculate shadow
float ShadowMap(){
    //FragLightSpacePos is in NDC but it doesnt divide by w
    vec4 NDCCoord = fs_in.FragLightSpacePos/fs_in.FragLightSpacePos.w;
    //this coord is frag coord in light space(between -1 and 1),but depthtex is between 0,1
    // so as UV coord
    NDCCoord = NDCCoord*0.5 + 0.5;
    float closestDepth = texture(shadowMap, NDCCoord.xy).r;
    float currentDepth = NDCCoord.z;
    //1 : in shadow 
    return currentDepth>closestDepth? 1:0;
}


void main()
{
    float shadow = 0;
    shadow = ShadowMap();

    FragColor = texture(basicTex0,fs_in.coord);
    //FragColor = vec4(1.0,0.0,0.0,1.0);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 spot = calculateSpotLight(spotLight,fs_in.Normal,viewDir);
    vec3 direct = calculateDirectLight(dirLight,fs_in.normal,viewDir,shadow);
    vec3 result = direct;
    //FragColor =vec4(FragPos,1.0f);
    FragColor = vec4(result,1.0f);
}

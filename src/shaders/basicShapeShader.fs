#version 430 core
layout (location = 0) out vec4 FragColor;



in VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
}fs_in;

uniform sampler2D shadowMap;
uniform sampler2D basicTex0;


struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 


uniform DirLight dirLight;
// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 halfVector = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, halfVector), 0.0), shininess);
}

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


float PCFShadowSmooth(vec3 NDCCoord){
    // plane is not like mesh
    // even it has to face it cannot avoid shadow scne, there is not distance between front and back
    float bias = max(0.001 * (1.0 - dot(fs_in.normal, normalize(-dirLight.direction))), 0.00001);
    float shadow = 0.0;
    vec2 size = 1/textureSize(shadowMap,0);
    for(int i=-1;i<=1;++i){
        for(int j=-1;j<=1;++j){
            float closestDepth = texture(shadowMap,NDCCoord.xy+vec2(i,j)*size).r;    // * is different from dot
            shadow += (NDCCoord.z-bias)>closestDepth? 1:0;
        }
    }
    shadow /= 9.0f;
    return shadow;
}


//calculate shadow
float ShadowMap(){
    //FragLightSpacePos is in NDC but it doesnt divide by w
    vec4 NDCCoord = fs_in.FragLightSpacePos/fs_in.FragLightSpacePos.w;
    //this coord is frag coord in light space(between -1 and 1),but depthtex is between 0,1
    // so as UV coord
    NDCCoord = NDCCoord*0.5 + 0.5;
    if(NDCCoord.z>1.0) return 0.0f;
    float shadow = PCFShadowSmooth(NDCCoord.xyz);
    return shadow;

    // fix shadow acne(angle between normal and lightdir is large bias should also be)
    //float bias = max(0.05 * (1.0 - dot(fs_in.normal, normalize(-dirLight.direction))), 0.005);
}

void main()
{
    float shadow = 0;
    shadow = ShadowMap();
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 spot = calculateSpotLight(spotLight,fs_in.Normal,viewDir);
    vec3 direct = calculateDirectLight(dirLight,fs_in.normal,viewDir,shadow);
    vec3 result = direct;
    //FragColor = texture(basicTex0,fs_in.coord);
    FragColor = vec4(result,1.0f);
}

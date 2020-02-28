#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

in VS_OUT{
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
}fs_in;


out vec4 FragColor;

//material
struct StandardMaterial{
    // link to sky(not good)
    samplerCube sky;
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};
//direct light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
//point light
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
//spot light
struct SpotLight{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //cosine of in/out angle
    float innerCutoff;
    float outerCutOff;
};


uniform StandardMaterial material;
uniform vec3 viewPos;
// struct StandardMaterial{
//     // color of each channel(can replace by texture)
//     sampler2D diffuse;
//     sampler2D specular;
//     float shininess;
// };
// uniform StandardMaterial material;
uniform DirLight dirLight;
uniform int PointNum;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int SpotNum;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,StandardMaterial material){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * texture(material.diffuse,fs_in.TexCoord).rgb;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * dirLight.specular * texture(material.specular,fs_in.TexCoord).rgb;

    //ambient
    vec3 ambient = dirLight.ambient * texture(material.diffuse,fs_in.TexCoord).rgb;
    return diffuse + specular + ambient;
}

//point light calculation
vec3 calculatePointLight(PointLight pointLight , vec3 Normal , vec3 viewDir, StandardMaterial material){
    vec3 lightDir =normalize(pointLight.position - fs_in.FragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * pointLight.diffuse * texture(material.diffuse,fs_in.TexCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * pointLight.specular * texture(material.specular,fs_in.TexCoord).rgb;

    //ambient
    vec3 ambient = pointLight.ambient * texture(material.diffuse,fs_in.TexCoord).rgb;

    //attenuation
    float dist = length(pointLight.position - fs_in.FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));

    vec3 result = diffuse + specular + ambient;
    result *= attenuation;

    return result;    
}

//spotlight calculation
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir, StandardMaterial material){
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * spotLight.diffuse * texture(material.diffuse,fs_in.TexCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * spotLight.specular * texture(material.specular,fs_in.TexCoord).rgb;

    //ambient
    vec3 ambient = spotLight.ambient * texture(material.diffuse,fs_in.TexCoord).rgb;

    //attenuation
    float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
    float delta = spotLight.innerCutoff - spotLight.outerCutOff;
    float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
    vec3 result = diffuse + specular + ambient;
    result *= attenuation;
    return result;    
}

// from screen depth to linear 01
float depthToEyeCoord(float ScreenDepth){
    float NDCDepth = ScreenDepth * 2 - 1;
    float depth = (2.0 * fs_in.Near * fs_in.Far) / (fs_in.Far + fs_in.Near - NDCDepth * (fs_in.Far - fs_in.Near));   
    return depth/fs_in.Far;
}


void main()
{

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 refDir = reflect(-viewDir,fs_in.Normal);
    //calculate the reflect light
    vec3 reflectColor = texture(material.ambient,fs_in.TexCoord).rgb * texture(material.sky,refDir).rgb;
    //calculate all lights
    vec3 point = vec3(0.0f);
    for(int i=0;i<min(PointNum,NUM_POINT_LIGHTS);i++){
        point += calculatePointLight(pointLights[i],fs_in.Normal,viewDir,material);
    }
    vec3 spot = vec3(0.0f);
    for(int i=0;i<min(SpotNum,NUM_SPOT_LIGHTS);i++){
        spot += calculateSpotLight(spotLights[i],fs_in.Normal,viewDir,material);
    }
    vec3 direct = vec3(0.0f);
    direct += calculateDirectLight(dirLight,fs_in.Normal,viewDir,material);

    vec3 result = spot + point + direct + reflectColor;

    

    //FragColor =vec4(FragPos,1.0f);
    FragColor = vec4(result,1.0f);
    //FragColor = texture(material.sky,ref);
}

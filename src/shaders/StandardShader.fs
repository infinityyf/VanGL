#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat4 View;
in mat4 Projection;
in float Near;
out float Far;

out vec4 FragColor;


uniform vec3 viewPos;
// struct StandardMaterial{
//     // color of each channel(can replace by texture)
//     sampler2D diffuse;
//     sampler2D specular;
//     float shininess;
// };
// uniform StandardMaterial material;


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
uniform StandardMaterial material;




//direct light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,StandardMaterial material){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * texture(material.diffuse,TexCoord).rgb;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * dirLight.specular * texture(material.specular,TexCoord).rgb;

    //ambient
    vec3 ambient = dirLight.ambient * texture(material.diffuse,TexCoord).rgb;
    return diffuse + specular + ambient;
}


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
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
uniform int PointNum;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
vec3 calculatePointLight(PointLight pointLight , vec3 Normal , vec3 viewDir, StandardMaterial material){
    vec3 lightDir =normalize(pointLight.position - FragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * pointLight.diffuse * texture(material.diffuse,TexCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * pointLight.specular * texture(material.specular,TexCoord).rgb;

    //ambient
    vec3 ambient = pointLight.ambient * texture(material.diffuse,TexCoord).rgb;

    //attenuation
    float dist = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));

    vec3 result = diffuse + specular + ambient;
    result *= attenuation;

    return result;    
}


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
#define NUM_SPOT_LIGHTS 4
uniform int SpotNum;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir, StandardMaterial material){
    vec3 lightDir = normalize(spotLight.position - FragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * spotLight.diffuse * texture(material.diffuse,TexCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * spotLight.specular * texture(material.specular,TexCoord).rgb;

    //ambient
    vec3 ambient = spotLight.ambient * texture(material.diffuse,TexCoord).rgb;

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
    float depth = (2.0 * Near * Far) / (Far + Near - NDCDepth * (Far - Near));   
    return depth/Far;
}


void main()
{

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 refDir = reflect(-viewDir,Normal);
    //calculate the reflect light
    vec3 reflectColor = texture(material.ambient,TexCoord).rgb * texture(material.sky,refDir).rgb;
    //calculate all lights
    vec3 point = vec3(0.0f);
    for(int i=0;i<min(PointNum,NUM_POINT_LIGHTS);i++){
        point += calculatePointLight(pointLights[i],Normal,viewDir,material);
    }
    vec3 spot = vec3(0.0f);
    for(int i=0;i<min(SpotNum,NUM_SPOT_LIGHTS);i++){
        spot += calculateSpotLight(spotLights[i],Normal,viewDir,material);
    }
    vec3 direct = vec3(0.0f);
    direct += calculateDirectLight(dirLight,Normal,viewDir,material);

    vec3 result = spot + point + direct + reflectColor;

    

    //FragColor =vec4(FragPos,1.0f);
    FragColor = vec4(result,1.0f);
    //FragColor = texture(material.sky,ref);
}

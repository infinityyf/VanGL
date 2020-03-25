#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

out vec4 FragColor;
in vec2 TexCoords;

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


// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,vec3 g_ambient,vec3 g_specualr,vec3 g_diffuse){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * g_diffuse;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = spec * dirLight.specular * g_specualr;

    //ambient
    vec3 ambient = dirLight.ambient * g_ambient;
    return diffuse + specular + ambient;
}

//point light calculation
vec3 calculatePointLight(PointLight pointLight , vec3 Normal , vec3 viewDir, vec3 g_ambient,vec3 g_specualr,vec3 g_diffuse,vec3 g_position){
    vec3 lightDir =normalize(pointLight.position - g_position);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * pointLight.diffuse * g_diffuse;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = spec * pointLight.specular * g_specualr;

    //ambient
    vec3 ambient = pointLight.ambient * g_ambient;

    //attenuation
    float dist = length(pointLight.position - g_position);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));

    vec3 result = diffuse + specular + ambient;
    result *= attenuation;

    return result;    
}

//spotlight calculation
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir, vec3 g_ambient,vec3 g_specualr,vec3 g_diffuse,vec3 g_position){
    vec3 lightDir = normalize(spotLight.position - g_position);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * spotLight.diffuse * g_diffuse;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = spec * spotLight.specular * g_specualr;

    //ambient
    vec3 ambient = spotLight.ambient * g_ambient;

    //attenuation
    float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
    float delta = spotLight.innerCutoff - spotLight.outerCutOff;
    float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
    vec3 result = diffuse + specular + ambient;
    result *= attenuation;
    return result;    
}

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform int PointNum;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int SpotNum;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];


// gbuffers
uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_Ambient;
uniform sampler2D g_Specular;
uniform sampler2D g_Diffuse;

void main()
{
    
    vec3 normal = texture(g_Normal,TexCoords).rgb;
    vec3 position = texture(g_Position,TexCoords).rgb;
    vec3 ambient = texture(g_Ambient,TexCoords).rgb;
    vec3 specualr = texture(g_Specular,TexCoords).rgb;
    vec3 diffuse = texture(g_Diffuse,TexCoords).rgb;

    //depth of frag
    float depth = texture(g_Position,TexCoords).a;

    vec3 viewDir = normalize(viewPos - position);
    vec3 refDir = reflect(-viewDir,normal);

    //calculate all lights
    vec3 point = vec3(0.0f);
    for(int i=0;i<min(PointNum,NUM_POINT_LIGHTS);i++){
        point += calculatePointLight(pointLights[i],normal,viewDir,ambient,specualr,diffuse,position);
    }
    vec3 spot = vec3(0.0f);
    for(int i=0;i<min(SpotNum,NUM_SPOT_LIGHTS);i++){
        spot += calculateSpotLight(spotLights[i],normal,viewDir,ambient,specualr,diffuse,position);
    }
    vec3 direct = vec3(0.0f);
    direct += calculateDirectLight(dirLight,normal,viewDir,ambient,specualr,diffuse);

    vec3 result = spot + point + direct;

    FragColor = vec4(result,1.0f);
    //FragColor = vec4(depth,depth,depth,1.0f);
}

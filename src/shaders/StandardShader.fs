#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct StandardMaterial{
    // color of each channel(can replace by texture)
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform StandardMaterial material;

struct Light{
    //control weight of different component
    vec3 position;
    //direct light and spot light needs direction
    vec3  direction;

    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //point light parameters
    float constant;
    float linear;
    float quadratic;

    //spot light range angle
    float innerCutoff;
    float outerCutOff;
};

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
    vec3 ambient = dirLight.ambient * material.ambient;
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
    vec3 ambient = pointLight.ambient * material.ambient;

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
    vec3 ambient = spotLight.ambient * material.ambient;

    //attenuation
    float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
    float delta = spotLight.innerCutoff - spotLight.outerCutOff;
    float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
    vec3 result = diffuse + specular + ambient;
    result *= attenuation;
    return result;    
}




uniform vec3 viewPos;
//later consider to support more than one light
uniform Light light;

out vec4 FragColor;

void main()
{
    //FragColor = texture(texture2,TexCoord);
    //FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);

    //claculate ambient
    vec3 ambient = texture(material.diffuse,TexCoord).rgb * light.ambient;

    //calculate diffuse in world coordinate
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal,lightDir),0.0f);
    vec3 diffuse = texture(material.diffuse,TexCoord).rgb * diff * light.diffuse;

    //calculate speqular light
    vec3 viewDir = normalize(viewPos - FragPos);
    // from light point to fragment
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = texture(material.specular,TexCoord).rgb * spec * light.specular;

    vec3 result = ambient + diffuse + specular;

    //calculate all lights


    FragColor = vec4(result,1.0f);
}

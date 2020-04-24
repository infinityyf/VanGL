//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

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
uniform DirLight dirLight;
uniform int PointNum;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int SpotNum;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,StandardMaterial material,float shadow,vec2 texCoord){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * texture(material.diffuse,texCoord).rgb;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * dirLight.specular * texture(material.specular,texCoord).rgb;

    //ambient
    vec3 ambient = dirLight.ambient * texture(material.diffuse,texCoord).rgb;
    return (diffuse + specular)*(1.0 - shadow) + ambient;
}

//point light calculation
vec3 calculatePointLight(PointLight pointLight , vec3 Normal , vec3 viewDir, StandardMaterial material,float shadow,vec3 fragPos,vec2 texCoord){
    vec3 lightDir =normalize(pointLight.position - fragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * pointLight.diffuse * texture(material.diffuse,texCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * pointLight.specular * texture(material.specular,texCoord).rgb;

    //ambient
    vec3 ambient = pointLight.ambient * texture(material.diffuse,texCoord).rgb;

    //attenuation
    float dist = length(pointLight.position - fragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));

    vec3 result = (diffuse + specular)*(1.0 - shadow) + ambient;
    result *= attenuation;

    return result;    
}

//spotlight calculation
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir, StandardMaterial material,float shadow,vec3 fragPos,vec2 texCoord){
    vec3 lightDir = normalize(spotLight.position - fragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * spotLight.diffuse * texture(material.diffuse,texCoord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * spotLight.specular * texture(material.specular,texCoord).rgb;

    //ambient
    vec3 ambient = spotLight.ambient * texture(material.diffuse,texCoord).rgb;

    //attenuation
    float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
    float delta = spotLight.innerCutoff - spotLight.outerCutOff;
    float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
    vec3 result = (diffuse + specular)*(1.0 - shadow) + ambient;
    result *= attenuation;
    return result;    
}
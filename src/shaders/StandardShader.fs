#version 330 core
//support more than one spotlight
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

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


//out vec4 FragColor;

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
uniform DirLight dirLight;
uniform int PointNum;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int SpotNum;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];
//shadow map
uniform sampler2D shadowMap;

// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,StandardMaterial material,float shadow){
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
    return (diffuse + specular)*(1.0 - shadow) + ambient;
}

//point light calculation
vec3 calculatePointLight(PointLight pointLight , vec3 Normal , vec3 viewDir, StandardMaterial material,float shadow){
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

    vec3 result = (diffuse + specular)*(1.0 - shadow) + ambient;
    result *= attenuation;

    return result;    
}

//spotlight calculation
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir, StandardMaterial material,float shadow){
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
    vec3 result = (diffuse + specular)*(1.0 - shadow) + ambient;
    result *= attenuation;
    return result;    
}

// from screen depth to linear 01
float depthToEyeCoord(float ScreenDepth){
    float NDCDepth = ScreenDepth * 2 - 1;
    float depth = (2.0 * fs_in.Near * fs_in.Far) / (fs_in.Far + fs_in.Near - NDCDepth * (fs_in.Far - fs_in.Near));   
    return depth/fs_in.Far;
}


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
float ShadowMap(vec3 normal){
    //FragLightSpacePos is in NDC but it doesnt divide by w
    vec4 NDCCoord = fs_in.FragLightSpacePos/fs_in.FragLightSpacePos.w;
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
    normal = fs_in.Normal;

    float shadow = 0;
    shadow = ShadowMap(normal);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 refDir = reflect(-viewDir,normal);
    //calculate the reflect light
    vec3 reflectColor = texture(material.ambient,fs_in.TexCoord).rgb * texture(material.sky,refDir).rgb;
    //calculate all lights
    vec3 point = vec3(0.0f);
    for(int i=0;i<min(PointNum,NUM_POINT_LIGHTS);i++){
        point += calculatePointLight(pointLights[i],normal,viewDir,material,shadow);
    }
    vec3 spot = vec3(0.0f);
    for(int i=0;i<min(SpotNum,NUM_SPOT_LIGHTS);i++){
        spot += calculateSpotLight(spotLights[i],normal,viewDir,material,shadow);
    }
    vec3 direct = vec3(0.0f);
    direct += calculateDirectLight(dirLight,normal,viewDir,material,shadow);

    vec3 result = spot + point + direct + reflectColor;

    FragColor = vec4(result,1.0f);
}

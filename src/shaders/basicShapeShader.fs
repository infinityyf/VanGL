#version 330 core
in VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
}fs_in;
out vec4 FragColor;  

uniform sampler2D basicTex0;
uniform vec3 viewPos;

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

uniform SpotLight spotLight;

// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//spotlight calculation
vec3 calculateSpotLight(SpotLight spotLight , vec3 Normal , vec3 viewDir){
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * spotLight.diffuse * texture(basicTex0,fs_in.coord).rgb;

    //specular
    vec3 reflectDir = reflect(-lightDir,Normal);
    //float spec = Phong(viewDir,reflectDir,32.0f);
    float spec = Blinn_Phong(viewDir,lightDir,Normal,8.0f);
    vec3 specular = spec * spotLight.specular * texture(basicTex0,fs_in.coord).rgb;

    //ambient
    vec3 ambient = spotLight.ambient * texture(basicTex0,fs_in.coord).rgb;

    //attenuation
    float theta =max(dot(-lightDir,normalize(spotLight.direction)),0.0f);
    float delta = spotLight.innerCutoff - spotLight.outerCutOff;
    float attenuation = clamp((theta-spotLight.outerCutOff)/delta,0.0f,1.0f);
    vec3 result = diffuse + specular + ambient;
    result *= attenuation;
    return result;    
}






void main()
{
    FragColor = texture(basicTex0,fs_in.coord);
    //FragColor = vec4(1.0,0.0,0.0,1.0);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    FragColor = vec4(calculateSpotLight(spotLight,fs_in.normal,viewDir),1.0);
}

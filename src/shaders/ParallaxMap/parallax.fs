#version 330 core
in VS_OUT{
    vec2 coord;
    vec3 normal;
    vec3 FragPos;
    vec4 FragLightSpacePos;
    mat3 TBN;
}fs_in;
out vec4 FragColor;  

uniform sampler2D basicTex;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform sampler2D shadowMap;
uniform vec3 viewPos;

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
    vec3 half = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, half), 0.0), shininess);
}

//directlight calculation
vec3 calculateDirectLight(DirLight dirLight , vec3 Normal , vec3 viewDir,vec2 texCoord,float shadow){
    vec3 lightDir = normalize(-dirLight.direction);
    //diffuse
    float diff = max(dot(lightDir,Normal),0.0f);
    vec3 diffuse = diff * dirLight.diffuse * texture(basicTex,texCoord).rgb;

    //speqular
    vec3 reflectDir = reflect(-lightDir,Normal);
    //float spec = Phong(viewDir,reflectDir,32.0f);
    float spec = Blinn_Phong(viewDir,lightDir,Normal,8.0f);
    vec3 specular = spec * dirLight.specular * texture(basicTex,texCoord).rgb;

    //ambient
    vec3 ambient = dirLight.ambient * texture(basicTex,texCoord).rgb;
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

vec2 ParalaxMap(vec3 viewDir){
    //get how deep to go
    float height =  texture(heightMap, fs_in.coord).r;
    //calculate the uv shift
    vec2 heightMapShift = viewDir.xy/viewDir.z * height * 0.1;
    return fs_in.coord-heightMapShift;
}

void main()
{
    //paralax map is calculate in tangent coord
    vec3 viewDirTangent = normalize(fs_in.TBN*viewPos - fs_in.TBN*fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec2 texcoord = ParalaxMap(viewDirTangent);

    vec3 normal  = texture(normalMap, texcoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);// change normal from tangent coord to world coord

    vec3 direct = calculateDirectLight(dirLight,normal,viewDir,texcoord,0);
    vec3 result = direct;
    FragColor = vec4(result,1.0f);
}

#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;
const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
//generate random number
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
} 

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));//这是通过重要性采样，在通过概率密度函数逆变换得到的结果（过程很复杂）
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = normalize(cross(N, tangent));

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;//translate to world coord
    return normalize(sampleVec);
}

void main(){
    vec3 N = normalize(localPos);    //这里利用不同角度观察，波瓣形状变换不大而做的一个近似，使法线，反射方向和视线方向一致（很聪明的近似，因为前两个量在预计算的时候无法得到）
    vec3 R = N;
    vec3 V = N;
    //GGX important sample
    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0); 
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT); 
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);//生成采样的方向
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);     //计算反射向量

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMap, L).rgb * NdotL;//带权求和取平均
            totalWeight      += NdotL;
        }  
    }
    prefilteredColor = prefilteredColor / totalWeight;  //将重要性采样求和式化简之后，去掉F（unity的做法），再将G项近似成cos（也就是这里的NDOTL），得到的式子

    FragColor = vec4(prefilteredColor, 1.0);
}


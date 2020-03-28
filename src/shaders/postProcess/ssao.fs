#version 330 core
out float FragColor;    //just one channel
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

layout(std140) uniform Samples{
    vec3 samples[64]; //64 * 16 = 768
};
//uniform vec3 samples[64];
uniform mat4 projection;
uniform mat4 view;
uniform int width;
uniform int height;



void main()
{
    vec2 noiseScale = vec2(width/4.0, height/4.0);

    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;              //position in world coord
    vec3 normal = texture(gNormal, TexCoords).rgb;                      //normal in world coord
    normal = normal*2.0 - 1.0;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;     

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));//Gramm-Schmidt Process
    vec3 bitangent =normalize(cross(tangent,normal));
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < 64; ++i)
    {
        vec3 sample = TBN * samples[i];     //just a coord translate
        sample = fragPos + sample * 0.01f;  //from tangent coord to world coord

        vec4 samplePoint = vec4(sample, 1.0);//???
        vec4 sampleView = view* samplePoint;
        float depth = -sampleView.z;
        samplePoint = projection *view* samplePoint;   //clip coord
        samplePoint.xyz /= samplePoint.w;
        samplePoint.xyz = samplePoint.xyz * 0.5 + 0.5;    //to 0-1


        float sampleDepth = texture(gPositionDepth, samplePoint.xy).w;//get mesh depth
        float rangeCheck = smoothstep(0.0, 1.0, 0.01 / abs(depth - sampleDepth));
        occlusion += (sampleDepth < depth ? 1.0 : 0.0) * rangeCheck;  
        //occlusion += samples[i].x;
    }
    FragColor =1- (occlusion /64);
    //FragColor = normal.x;
}
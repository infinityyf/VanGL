#version 330 core
out float FragColor;    //just one channel
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

layout(std140) uniform Samples{
    vec3 samples[64]; //64 * 12 = 768
};
//uniform vec3 samples[64];
uniform mat4 projection;
uniform int width;
uniform int height;



void main()
{
    vec2 noiseScale = vec2(width/4.0, height/4.0);

    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;              //position in world coord
    vec3 normal = texture(gNormal, TexCoords).rgb;                      //normal in world coord
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;     

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));//Gramm-Schmidt Process
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < 64; ++i)
    {
        vec3 sample = TBN * samples[i];     //just a coord translate
        sample = fragPos + sample * 1.0f;  //from tangent coord to world coord

        vec4 offset = vec4(sample, 1.0);//???
        offset = projection * offset;   //clip coord
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;    //to 0-1


        float sampleDepth = texture(gPositionDepth, offset.xy).w;//get depth
        occlusion += (sampleDepth < sample.z ? 1.0 : 0.0);
    }
    //FragColor = occlusion / 64;
    FragColor = fragPos.x;
}
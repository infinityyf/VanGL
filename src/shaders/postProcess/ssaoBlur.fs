#version 330 core
out float FragColor;    //just one channel
in vec2 TexCoords;

uniform sampler2D ssaoTexture;

void main()
{
    vec2 size = 1/textureSize(ssaoTexture,0);
    float blurColor = 0.0;
    for(int i=-2;i<=2;i++){
        for(int j=-2;j<=2;j++){
            vec2 offset = vec2(i,j) * size;
            blurColor += texture(ssaoTexture,offset).r;
        }
    }
    blurColor /= 16;
    FragColor = blurColor;
}
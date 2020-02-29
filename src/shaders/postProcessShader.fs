#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;


void main()
{ 
    float gamma = 2.2f;
    FragColor = texture(screenTexture, TexCoords);
    FragColor = pow(FragColor,vec4(gamma));
}
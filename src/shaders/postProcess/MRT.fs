#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;    //do nothing just show the picture
uniform sampler2D bloomTexture;
//uniform float exposure;

void main()
{ 
    float gamma = 2.2f;
    vec3 sceneColor = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb; 
    sceneColor += bloomColor;
    
	//HDR
    //reinhard tone mapping
    //sceneColor = sceneColor/(sceneColor+vec3(1.0));
    //exposure
    //color = vec3(1.0) - exp(-color*exposure)

    //gamma correction
    sceneColor = pow(sceneColor,vec3(1.0/gamma));

    FragColor = vec4(sceneColor,1.0);
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;    //do nothing just show the
//uniform float exposure;

void main()
{ 
    float gamma = 2.2f;
    vec3 color = texture(screenTexture, TexCoords).rgb;

	//HDR
    //reinhard tone mapping
    //color = color/(color+vec3(1.0));
    //exposure
    //color = vec3(1.0) - exp(-color*exposure)

    //gamma correction
    //color = pow(color,vec4(1.0/gamma));

    FragColor = vec4(color,1.0);
}
#version 330 core
out vec4 FragColor;  
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;

void main()
{
    //FragColor = texture(texture2,TexCoord);
    FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);

    float ambientStrength = 0.1f;
    vec3 ambient = lightColor * ambientStrength;

    vec3 result = ambient * FragColor.rgb;
    FragColor = vec4(result,1.0f);
}

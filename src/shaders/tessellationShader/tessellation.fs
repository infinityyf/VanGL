#version 430 core
out vec4 FragColor;
in vec3 height;
in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
uniform sampler2D terrainTex0;
uniform sampler2D terrainTex1;
uniform sampler2D terrainTex2;
void main(){

    vec3 color0 = texture(terrainTex0,TexCoord_FS_in*128).rgb;
    vec3 color1 = texture(terrainTex1,TexCoord_FS_in*128).rgb;
    vec3 color2 = texture(terrainTex2,TexCoord_FS_in*128).rgb;
    vec3 color = mix(color1,color0,height.y/1.0f);
    FragColor = vec4(color,1.0f);
}
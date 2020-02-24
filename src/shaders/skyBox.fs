#version 330 core
out vec4 FragColor;
in vec3 TexCoords; // 代表3D纹理坐标的方向向量
uniform samplerCube cubemap; // 立方体贴图的纹理采样器

void main()
{             
    gl_FragDepth = 1.0f;
    FragColor = texture(cubemap, TexCoords);
}
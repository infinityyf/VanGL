#version 410 core
layout(triangles, equal_spacing, cw) in;
in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];
out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;

out vec3 height;

struct Camera{          //align
    mat4 view;          //64    //0
    mat4 projection;    //64    //64
    float near;         //4     //128
    float far;          //4     //132
};                              //144
//use uniform block we can access component directly nont need to use Matrix
layout(std140) uniform Matrix{
    Camera camera;      //144   //0
    mat4 lightSpaceMatrix;//64    //208
    
};

uniform sampler2D heightMap;
uniform float heightScale;
//v0,v1,v2 come from tcl
//gl_tessCoord comes from pg
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
} 

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    TexCoord_FS_in = interpolate2D(TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2]);
    Normal_FS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
    Normal_FS_in = normalize(Normal_FS_in);
    WorldPos_FS_in = interpolate3D(WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2]);

     // Displace the vertex along the normal
    WorldPos_FS_in += vec3(0.0,texture(heightMap,TexCoord_FS_in).r*heightScale,0.0);
    height = texture(heightMap,TexCoord_FS_in).rrr;
    gl_Position = camera.projection*camera.view * vec4(WorldPos_FS_in, 1.0);
} 

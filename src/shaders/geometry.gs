#version 330 core
layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;


in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
} gs_in[];


// this block with assign in each point in output primitive
// donnt need gsout[3]
// assign before EmitVertex();
out GS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoord;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
} gs_out;


uniform float time;

vec3 GetNormal(){
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 pos,vec3 nor,float time){
    float magnitude = 2.0;
    vec3 direction = nor * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return pos + vec4(direction, 0.0);
}


void main(){
    vec3 normal = GetNormal();
    gl_Position = explode(gl_in[0].gl_Position,normal,time);
    gs_out.TexCoord = gs_in[0].TexCoord;
    gs_out.FragPos = gs_in[0].FragPos;
    gs_out.Normal = gs_in[0].Normal;
    gs_out.View = gs_in[0].View;
    gs_out.Projection = gs_in[0].Projection;
    gs_out.Near = gs_in[0].Near;
    gs_out.Far = gs_in[0].Far;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position,normal,time);
    gs_out.TexCoord = gs_in[1].TexCoord;
    gs_out.FragPos = gs_in[1].FragPos;
    gs_out.Normal = gs_in[1].Normal;
    gs_out.View = gs_in[1].View;
    gs_out.Projection = gs_in[1].Projection;
    gs_out.Near = gs_in[1].Near;
    gs_out.Far = gs_in[1].Far;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position,normal,time);
    gs_out.TexCoord = gs_in[2].TexCoord;
    gs_out.FragPos = gs_in[2].FragPos;
    gs_out.Normal = gs_in[2].Normal;
    gs_out.View = gs_in[2].View;
    gs_out.Projection = gs_in[2].Projection;
    gs_out.Near = gs_in[2].Near;
    gs_out.Far = gs_in[2].Far;
    EmitVertex();
    EndPrimitive();

    
    
    
}
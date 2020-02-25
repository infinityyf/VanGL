#version 330 core
layout(triangles) in;
layout(line_strip,max_vertices = 6) out ;

in VS_OUT{
    vec3 Normal;
}gs_in[];

void main(){
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(gs_in[0].Normal,1.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(gs_in[1].Normal,1.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + vec4(gs_in[2].Normal,1.0);
    EmitVertex();
    EndPrimitive();
}
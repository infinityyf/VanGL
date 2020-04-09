#version 330 core
layout(triangles) in;
layout(line_strip,max_vertices = 18) out ;

in VS_OUT{
    vec3 bitangent;
    vec3 normal;
    vec3 tangent;
}gs_in[];

void main(){
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + 0.02f*vec4(gs_in[0].tangent,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + 0.02f*vec4(gs_in[0].normal,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + 0.02f*vec4(gs_in[0].bitangent,0.0);
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + 0.02f*vec4(gs_in[1].tangent,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + 0.02f*vec4(gs_in[1].normal,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + 0.02f*vec4(gs_in[1].bitangent,0.0);
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + 0.02f*vec4(gs_in[2].tangent,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + 0.02f*vec4(gs_in[2].normal,0.0);
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + 0.02f*vec4(gs_in[2].bitangent,0.0);
    EmitVertex();
    EndPrimitive();
}
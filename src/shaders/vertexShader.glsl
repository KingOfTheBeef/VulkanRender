#version 450

layout(set=0, binding=1) uniform uniformBuffer1 {
    mat4 projection;
};

layout(set=0, binding=2) uniform uniformBuffer2 {
    mat4 models[2];
};

// Vertex data
layout(location = 0) in vec4 vert_Pos;
layout(location = 1) in vec2 vert_Texcoord;

// Instance data
layout(location = 2) in vec2 inst_Pos;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    gl_Position = projection * (vert_Pos + vec4(inst_Pos, 0.0f, 0.0f));
    v_Texcoord = vert_Texcoord;
}
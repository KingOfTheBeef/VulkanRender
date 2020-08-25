#version 450

layout(set=0, binding=1) uniform uniformBuffer1 {
    mat4 projection;
};

layout(set=0, binding=2) uniform uniformBuffer2 {
    mat4 models[2];
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    gl_Position = projection * models[gl_InstanceIndex] * i_Position;
    v_Texcoord = i_Texcoord;
}
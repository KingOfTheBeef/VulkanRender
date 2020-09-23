#version 450

layout(set=0, binding=0) uniform uniformBuffer1 {
    mat4 projection;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in mat4 i_Model;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;

void main() {
    gl_Position = projection * i_Model * i_Position;
    v_Color = i_Color;
}
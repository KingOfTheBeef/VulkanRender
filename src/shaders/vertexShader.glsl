#version 450

layout(set=0, binding=1) uniform bufferVals {
    mat4 mat;
} u_UniformBuffer[2];

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    mat4 mat0 = u_UniformBuffer[0].mat;
    mat4 mat1 = u_UniformBuffer[1].mat;

    gl_Position = mat1 * mat0 * i_Position;
    v_Texcoord = i_Texcoord;
}
#version 450

layout(set=0, binding=1) uniform bufferVals {
    mat4 matrices[2];
} u_UniformBuffer[2];

mat4 proj = u_UniformBuffer[0].matrices[0];

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    mat4 model = u_UniformBuffer[1].matrices[gl_InstanceIndex];

    gl_Position = proj * model * i_Position;
    v_Texcoord = i_Texcoord;
}
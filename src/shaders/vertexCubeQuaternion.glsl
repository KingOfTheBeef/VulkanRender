#version 450

layout(set=0, binding=0) uniform uniformBuffer1 {
    mat4 projection;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec4 i_Rotation;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;

vec4 quaternionMult(vec4 p, vec4 q) {
    return vec4(p.x * q.x - dot(p.yzw, q.yzw), p.x * q.yzw + q.x * p.yzw + cross(p.yzw, q.yzw));
}

void main() {
    vec4 pos = quaternionMult(i_Rotation, quaternionMult(vec4(0, i_Position.xyz), vec4(i_Rotation.x, -i_Rotation.yzw)));
    gl_Position = projection * vec4(pos.yzw, 1);

    v_Color = i_Color;
}

#version 450

layout(location = 1) in vec4 colo;
layout(location = 0) in vec4 posi;
layout(location = 0) out vec4 v_Color;

void main() {
    // vec2 pos[3] = vec2[3]( vec2(-0.7, 0.7), vec2(0.7, 0.7), vec2(0.0, -0.7) );
    gl_Position = posi;
    v_Color = colo;
}
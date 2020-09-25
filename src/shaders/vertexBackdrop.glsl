#version 400

void main() {
    vec2 pos[6] = vec2[6]( vec2(-0.8, -0.8), vec2(0.8, -0.8), vec2(0.8, 0.8), vec2(-0.8, -0.8), vec2(-0.8, 0.8), vec2(0.8, 0.8) );
    gl_Position = vec4( pos[gl_VertexIndex], 1.0, 1.0 );
}
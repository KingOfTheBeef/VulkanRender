#version 450

layout(location = 0) in vec4 in_Color;
layout(location = 0) out vec4 out_Color;
void main() {
  out_Color = in_Color; // vec4( 1.000, 0.412, 0.706, 1.0 );
}
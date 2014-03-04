#version 120
attribute vec2 vertex_position;
attribute vec3 vertex_color;

varying vec3 frag_color;

void main() {
    gl_Position = vec4(vertex_position, 0.9, 1.0);
    frag_color = vertex_color;
}

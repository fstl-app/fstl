#version 120
attribute vec3 vertex_position;
attribute vec3 vertex_color;

uniform mat4 transform_matrix;
uniform mat4 view_matrix;

varying vec3 frag_color;

void main() {
    gl_Position = view_matrix*transform_matrix*
        vec4(vertex_position, 1.0);
    frag_color = vertex_color;
}

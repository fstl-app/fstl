#version 120
attribute vec3 vertex_position;

uniform mat4 transform_matrix;
uniform mat4 view_matrix;

varying vec3 ec_pos;

void main() {
    gl_Position = view_matrix*transform_matrix*
        vec4(vertex_position, 1.0);
    ec_pos = gl_Position.xyz;
}

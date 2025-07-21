#version 140
attribute vec3 vertex_position;

uniform mat4 transform_matrix;
uniform mat4 view_matrix;
uniform mat4 cutting_plane_matrix;

varying vec3 ec_pos;

void main() {
    vec4 origpos = transform_matrix*vec4(vertex_position, 1.0);
    gl_Position = view_matrix*origpos;
    ec_pos = gl_Position.xyz;

    vec4 vPos   = view_matrix * transform_matrix* vec4(vertex_position, 1.0);

    vec4 u_plane0       = transpose(inverse(cutting_plane_matrix))*vec4(0,0,1,0);
    gl_ClipDistance[0] = dot(u_plane0, origpos); //If you want to clip using the camera coordinate system
    //gl_ClipDistance[0] = dot(u_plane0, in_Vertex); //If you want to clip using the local coordinate system
	//gl_ClipDistance[0] = dot(u_plane0, camMat * modelViewM * in_Vertex); //If you want it in global system
}
